#include "fileHandler.h"
#include "tools.h"
#include <filesystem>
#include <fstream>

constexpr unsigned int chunkPack = 4;


void ChunkFileHandler::loadChunk(Chunk & c)
{


}

void ChunkFileHandler::saveChunk(Chunk & c)
{
	const glm::ivec2 pos = { c.position.x, c.position.z };
	
	glm::vec2 floatPos = pos;
	floatPos /= chunkPack;
	
	const glm::ivec2 filePos = { floorf(floatPos.x), floorf(floatPos.y) };

	char fileName[255];
	
	sprintf(fileName, "saves/c%d_%d.chunks", filePos.x, filePos.y);

	wlog(fileName);

	std::filesystem::create_directory("saves");

	if(!std::filesystem::exists(fileName))
	{
		std::ofstream f(fileName, std::ios::out | std::ios::binary);

		f.write((char*)&filePos, sizeof(filePos));

		//unsigned int terminal = 0xFFFFFFFF;
		//f.write((char*)terminal, 4);
		

		f.close();
	}else
	{
		std::fstream f;
		f.open(fileName);
		f.seekp(0);
		f.seekg(0);
		f.write((char*)&filePos.x, sizeof(filePos.x));
		f.close();

	}

}
