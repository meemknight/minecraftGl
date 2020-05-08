#pragma once
#include <fstream>
#include "Chunk.h"
#include "glm/vec2.hpp"


struct ChunkFileHandler
{
	//returns false if chunk hasn't been saved
	bool loadChunk(Chunk &c);
	void saveChunk(Chunk &c);
	void savePlayer(const glm::vec3 &pos);
	bool loadPlayer(glm::vec3 &pos);

private:
	void saveChunkDataInFile(std::fstream &f, Chunk &c, int index);
	void appendChunkDataInFile(std::fstream &f, Chunk &c);
	void loadChunkAtIndex(std::fstream &f, Chunk &c, int index);
};
