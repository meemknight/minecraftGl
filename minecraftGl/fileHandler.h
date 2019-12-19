#pragma once
#include <fstream>
#include "Chunk.h"
#include "glm/vec2.hpp"


struct ChunkFileHandler
{
	//returns false if chunk hasn't been saved
	bool loadChunk(Chunk &c);
	void saveChunk(Chunk &c);

private:
	void saveChunkDataInFile(std::fstream &f, Chunk &c, int index);
	void appendChunkDataInFile(std::fstream &f, Chunk &c);
	void loadChunkAtIndex(std::fstream &f, Chunk &c, int index);
};
