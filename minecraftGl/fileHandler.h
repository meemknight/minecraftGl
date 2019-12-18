#pragma once
#include <fstream>
#include "Chunk.h"
#include "glm/vec2.hpp"


struct ChunkFileHandler
{
	void loadChunk(Chunk &c);
	void saveChunk(Chunk &c);
};
