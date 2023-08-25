#include "Chunk.h"
#include "tools.h"

constexpr static int DRAW_EDGE_CHUNKS = 0;
constexpr static int DRAW_BOTTOM_CHUNKS = 0;

static inline void setFace(int x, int y, int z, FloatVector fv[], Chunk &c, Face f, glm::vec3 chunkPosition)
{
	if(isHerb(c.getBlock(x, y, z)))
	{
		auto face = getBlockFace((c.getBlock(x, y, z)), FACE::front);
		fv[FACE::FACES_SIZE].push(glm::vec3(x, y, z) + chunkPosition);
		fv[FACE::FACES_SIZE].push(face.x, face.y);
	}else
	{
		auto face = getBlockFace((c.getBlock(x, y, z)), f);
		fv[f].push(glm::vec3(x, y, z) + chunkPosition);
		fv[f].push(face.x, face.y);
	}
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

//note this does not check if chunk is built !!!
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
				if (getBlock(x,y,z) != BLOCK::air && !isHerb(getBlock(x, y, z)))
				{
					//top
					if (y < BUILD_LIMIT - 1)
					{
						if (!isOpaque(getBlock(x, y+1, z)))
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
						if (!isOpaque(getBlock(x, y-1, z)))
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
						if (!isOpaque(getBlock(x, y, z+1)))
						{
							setFace(x, y, z, positionData, *this, FACE::front, chunkPosition);
						}
					}
					else
					{
						if (neighbours[CN::front])
						{
							if (!isOpaque(neighbours[CN::front]->getBlock(x, y, 0)))
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
						if (!isOpaque(getBlock(x, y, z-1)))
						{
							setFace(x, y, z, positionData, *this, FACE::back, chunkPosition);
						}
					}
					else
					{
						if (neighbours[CN::back])
						{
							if (!isOpaque(neighbours[CN::back]->getBlock(x, y, 15)))
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
						if (!isOpaque(getBlock(x - 1, y, z)))
						{
							setFace(x, y, z, positionData, *this, FACE::left, chunkPosition);
						}
					}
					else
					{
						if (neighbours[CN::left])
						{
							if (!isOpaque(neighbours[CN::left]->getBlock(15, y, z)))
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
						if (!isOpaque(getBlock(x + 1, y, z)))
						{
							setFace(x, y, z, positionData, *this, FACE::right, chunkPosition);
						}
					}
					else
					{
						if (neighbours[CN::right])
						{
							if (!isOpaque(neighbours[CN::right]->getBlock(0, y, z)))
							{
								setFace(x, y, z, positionData, *this, FACE::right, chunkPosition);
							}
						}
						else if (DRAW_EDGE_CHUNKS)
						{
							setFace(x, y, z, positionData, *this, FACE::right, chunkPosition);
						}
					}
				}else if(isHerb(getBlock(x, y, z)))
				{
					//herbs
					setFace(x, y, z, positionData, *this, FACE::front, chunkPosition);
				}
			}
		}
	}

}
