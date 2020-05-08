#include "fileHandler.h"
#include "tools.h"
#include <filesystem>
#include <fstream>

constexpr unsigned int CHUNK_PACK = 4;

constexpr int chunkDataSize = CHUNK_SIZE * BUILD_LIMIT * CHUNK_SIZE * sizeof(Block);
constexpr int headDist = 1 + (CHUNK_PACK * CHUNK_PACK * sizeof(glm::ivec2));

//////////////////////Chunks//format/////////////////////////////////////////
//
//	(     char     ) (			glm::ivec2 8bytes		 ) ( unsigned char ) ( unsigned char )
//	/* 1 element  */ /* CHUNK_PACK*CHUNK_PACK elements	*/ /*  1 element  */ /*chunkDataSize*/
//	/*chunks count*/ /*			Chunk positions			*/ /* fully loaded*/ /* chunk data  */
//
/////////////////////////////////////////////////////////////////////////////
bool ChunkFileHandler::loadChunk(Chunk & c)
{
	//todo move into a function
	const glm::ivec2 pos = { c.position.x, c.position.z };

	glm::vec2 floatPos = pos;
	floatPos /= CHUNK_PACK;

	const glm::ivec2 filePos = { floorf(floatPos.x), floorf(floatPos.y) };

	char fileName[255];

	sprintf(fileName, "saves/c%d_%d.chunks", filePos.x, filePos.y);

	if (!std::filesystem::exists(fileName))
	{
		return 0;
	}

	//if it does exist, we first read it's content
	std::fstream f(fileName, std::ios::in | std::ios::out | std::ios::binary);

	f.seekg(0);

	char count = 0;
	f.read(&count, 1);

	if (count > CHUNK_PACK * CHUNK_PACK)
	{
		elog("corrupted file");
	}

	glm::ivec2 positions[CHUNK_PACK * CHUNK_PACK];

	f.read((char*)positions, count * sizeof(glm::ivec2));

	//then we see if that chunk was already loaded
	int loadIndex = -1;
	for (int i = 0; i < count; i++)
	{
		if (positions[i] == pos)
		{
			loadIndex = i;
			break;
		}
	}

	if(loadIndex == -1)
	{
		//chunk file exists but there is no such chunk generate new chunk...
		f.close();
		return 0;
	}

	loadChunkAtIndex(f, c, loadIndex);
	
	f.close();
	return 1;
}

void ChunkFileHandler::saveChunk(Chunk & c)
{
	const glm::ivec2 pos = { c.position.x, c.position.z };
	
	glm::vec2 floatPos = pos;
	floatPos /= CHUNK_PACK;
	
	const glm::ivec2 filePos = { floorf(floatPos.x), floorf(floatPos.y) };

	char fileName[255];
	
	sprintf(fileName, "saves/c%d_%d.chunks", filePos.x, filePos.y);

	if(!std::filesystem::exists(fileName))
	{	
		//if the chunk doesn't exist, we create it
		std::fstream f;
		f.open(fileName, std::ios::out | std::ios::binary);

		unsigned char fillData = 0xFF;

		char count = 1;

		f.write(&count, 1);

		for (int i = 0; i < CHUNK_PACK *CHUNK_PACK * sizeof(glm::ivec2); i++)
		{
			f.write((char*)&fillData, sizeof(fillData));
		}
		
		f.seekp(1);
		//f.seekg(1);

		f.write((char*)&pos, sizeof(pos));

		appendChunkDataInFile(f, c);

		f.close();
	}else
	{
		//if it does exist, we first read it's content
		std::fstream f(fileName, std::ios::in | std::ios::out | std::ios::binary);
		
		f.seekg(0);

		char count = 0;
		f.read(&count, 1);

		if(count > CHUNK_PACK * CHUNK_PACK)
		{
			elog("corrupted file");
		}

		glm::ivec2 positions[CHUNK_PACK * CHUNK_PACK];

		f.read((char*)positions, count * sizeof(glm::ivec2));

		//then we see if that chunk was already loaded
		int loadIndex = -1;
		for (int i = 0; i < count; i++)
		{
			if(positions[i] == pos)
			{
				loadIndex = i;
				break;
			}
		}

		if(loadIndex != -1)
		{
			saveChunkDataInFile(f, c, loadIndex);
		}else
		{
			//we add the new chunk
			f.seekp(0, std::ios_base::beg);
			count++;
			f.write(&count, 1);

			f.seekp(1 + (count-1) * sizeof(glm::ivec2), std::ios_base::beg);
			f.write((char*)&pos, sizeof(glm::ivec2));

			appendChunkDataInFile(f, c);
		}

		//const char* test = "12345678";
		//f.write(test, 8);
		//f.write((char*)&pos.x, sizeof(pos.x));
		f.close();
	}

}

void ChunkFileHandler::savePlayer(const glm::vec3 & pos)
{
	std::ofstream f("saves/player.txt");
	if(!f.is_open())
	{
		return;
	}

	f << pos.x << " ";
	f << pos.y << " ";
	f << pos.z;

	f.close();
}

bool ChunkFileHandler::loadPlayer(glm::vec3 & pos)
{
	std::ifstream f("saves/player.txt");
	if (!f.is_open())
	{
		return false;
	}

	f >> pos.x;
	f >> pos.y;
	f >> pos.z;

	f.close();

	return true;
}

void ChunkFileHandler::saveChunkDataInFile(std::fstream & f, Chunk & c, int index)
{	
	f.seekp(headDist + ((chunkDataSize + 1) * index), std::ios_base::beg);
	f.write((char*)&c.fullyLoaded, 1);
	f.write((char*)c.blockData, chunkDataSize);
}

void ChunkFileHandler::appendChunkDataInFile(std::fstream & f, Chunk & c)
{
	f.seekp(0, std::ios_base::end);
	f.write((char*)&c.fullyLoaded, 1);
	f.write((char*)c.blockData, chunkDataSize);
}

void ChunkFileHandler::loadChunkAtIndex(std::fstream & f, Chunk & c, int index)
{
	f.seekg(headDist + ((chunkDataSize+1) * index), std::ios_base::beg);
	f.read((char*)&c.fullyLoaded, 1);
	f.read((char*)c.blockData, chunkDataSize);
}
