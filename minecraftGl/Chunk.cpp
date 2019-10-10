#include "Chunk.h"

constexpr static int DRAW_EDGE_CHUNKS = 0;
constexpr static int DRAW_BOTTOM_CHUNKS = 0;

inline static void setFace(int x, int y, int z, FloatVector fv[], Chunk &c, Face f, glm::vec3 chunkPosition)
{
	auto face = getBlockFace((c.blockData[x][y][z]), f);
	fv[f].push(glm::vec3(x, y, z) + chunkPosition);
	fv[f].push(face.x, face.y);
}

void Chunk::removeNeighboursLinkage()
{
	if(neighbours[CN::front] != nullptr)
	{
		neighbours[CN::front]->neighbours[CN::back] = nullptr;
	}

	if (neighbours[CN::back] != nullptr)
	{
		neighbours[CN::back]->neighbours[CN::front] = nullptr;
	}

	if (neighbours[CN::left] != nullptr)
	{
		neighbours[CN::left]->neighbours[CN::right] = nullptr;
	}

	if (neighbours[CN::right] != nullptr)
	{
		neighbours[CN::right]->neighbours[CN::left] = nullptr;
	}
}

void Chunk::bakeMeshes()
{
	shouldRecreate = false;

	glm::vec3 chunkPosition = getPositionInUnits();

	resetMeshes();

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < BUILD_LIMIT; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				if (blockData[x][y][z] != BLOCK::air)
				{
					//top
					if (y < BUILD_LIMIT - 1)
					{
						if (!isSolid(blockData[x][y + 1][z]))
						{
							setFace(x, y, z, positionData, *this, FACE::top, chunkPosition);
						}
					}
					else
					{
						setFace(x, y, z, positionData, *this, FACE::top, chunkPosition);
					}

					//bottom
					if (y > 0)
					{
						if (!isSolid(blockData[x][y - 1][z]))
						{
							setFace(x, y, z, positionData, *this, FACE::bottom, chunkPosition);

						}
					}
					else
					{
						if (DRAW_BOTTOM_CHUNKS)
						{
							setFace(x, y, z, positionData, *this, FACE::bottom, chunkPosition);
						}
					}

					//front
					if (z < 15)
					{
						if (!isSolid(blockData[x][y][z + 1]))
						{
							setFace(x, y, z, positionData, *this, FACE::front, chunkPosition);
						}
					}
					else
					{
						if (neighbours[CN::front])
						{
							if (!isSolid(neighbours[CN::front]->blockData[x][y][0]))
							{
								setFace(x, y, z, positionData, *this, FACE::front, chunkPosition);
							}
						}
						else if (DRAW_EDGE_CHUNKS)
						{
							setFace(x, y, z, positionData, *this, FACE::front, chunkPosition);
						}
					}

					//back
					if (z > 0)
					{
						if (!isSolid(blockData[x][y][z - 1]))
						{
							setFace(x, y, z, positionData, *this, FACE::back, chunkPosition);
						}
					}
					else
					{
						if (neighbours[CN::back])
						{
							if (!isSolid(neighbours[CN::back]->blockData[x][y][15]))
							{
								setFace(x, y, z, positionData, *this, FACE::back, chunkPosition);
							}
						}
						else if (DRAW_EDGE_CHUNKS)
						{
							setFace(x, y, z, positionData, *this, FACE::back, chunkPosition);
						}

					}

					//left
					if (x > 0)
					{
						if (!isSolid(blockData[x - 1][y][z]))
						{
							setFace(x, y, z, positionData, *this, FACE::left, chunkPosition);
						}
					}
					else
					{
						if (neighbours[CN::left])
						{
							if (!isSolid(neighbours[CN::left]->blockData[15][y][z]))
							{
								setFace(x, y, z, positionData, *this, FACE::left, chunkPosition);
							}
						}
						else if (DRAW_EDGE_CHUNKS)
						{
							setFace(x, y, z, positionData, *this, FACE::left, chunkPosition);
						}

					}

					//right
					if (x < 15)
					{
						if (!isSolid(blockData[x + 1][y][z]))
						{
							setFace(x, y, z, positionData, *this, FACE::right, chunkPosition);
						}
					}
					else
					{
						if (neighbours[CN::right])
						{
							if (!isSolid(neighbours[CN::right]->blockData[0][y][z]))
							{
								setFace(x, y, z, positionData, *this, FACE::right, chunkPosition);
							}
						}
						else if (DRAW_EDGE_CHUNKS)
						{
							setFace(x, y, z, positionData, *this, FACE::right, chunkPosition);
						}
					}
				}
			}
		}
	}

}
