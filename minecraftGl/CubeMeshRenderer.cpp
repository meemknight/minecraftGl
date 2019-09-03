#include "CubeMeshRenderer.h"
constexpr float r = 0.5f;

constexpr int DRAW_EDGE_CHUNKS = 1;

#pragma region cubeData

float frontCubeData[] =
{
	-1.0f*r, 1.0f*r, 1.0*r,//pos
	 0.0f, 1.0f,//texcoord

	-1.0f*r, -1.0f*r, 1.0*r,
	 0.0f,  0.0f,

	 1.0f*r, -1.0f*r, 1.0*r,
	 1.0f,  0.0f,

	 1.0f*r, 1.0f*r, 1.0*r,
	 1.0f, 1.0f,
};

float backCubeData[] = 
{
	-1.0f*r, 1.0f*r, -1.0*r,//pos
	 1.0f, 1.0f,//texcoord

	-1.0f*r, -1.0f*r, -1.0*r,
	 1.0f,  0.0f,

	 1.0f*r, -1.0f*r, -1.0*r,
	 0.0f,  0.0f,

	 1.0f*r, 1.0f*r, -1.0*r,
	 0.0f, 1.0f,
};

float topCubeData[] =
{
		-1.0f*r, 1.0f*r, -1.0*r,
		0.0f, 1.0f,

		-1.0f*r, 1.0f*r, 1.0*r,
		0.0f*r, 0.0f*r,

		1.0f*r, 1.0f*r, 1.0*r,
		1.0f, 0.0f,

		1.0f*r, 1.0f*r, -1.0*r,
		1.0f, 1.0f,
};

float bottomCubeData[] =
{
		-1.0f*r, -1.0f*r, -1.0*r,
		0.0f, 1.0f,

		-1.0f*r, -1.0f*r, 1.0*r,
		0.0f, 0.0f,

		1.0f*r, -1.0f*r, 1.0*r,
		1.0f, 0.0f,

		1.0f*r, -1.0f*r, -1.0*r,
		1.0f, 1.0f,
};

float leftCubeData[] = 
{
	-1.0f*r, 1.0f*r, -1.0*r,
	 0.0f, 1.0f,

	 -1.0f*r, -1.0f*r, -1.0*r,
	 0.0f, 0.0f,

	 -1.0f*r, -1.0f*r, 1.0*r,
	 1.0f, 0.0f,

	 -1.0f*r, 1.0f*r, 1.0*r,
	 1.0f, 1.0f,
};

float rightCubeData[] =
{
	1.0f*r, 1.0f*r, -1.0*r,
	 1.0f, 1.0f,

	 1.0f*r, -1.0f*r, -1.0*r,
	 1.0f, 0.0f,

	 1.0f*r, -1.0f*r, 1.0*r,
	 0.0f, 0.0f,

	 1.0f*r, 1.0f*r, 1.0*r,
	 0.0f, 1.0f,
};

unsigned int frontIndexBufferData[] =
{
	0,
	1,
	3,
	3,
	1,
	2,
};

unsigned int backIndexBufferData[] =
{
	3,
	1,
	0,
	3,
	2,
	1,
};
#pragma endregion

float *cubeData[FACE::FACES_SIZE]{ frontCubeData, backCubeData, topCubeData, bottomCubeData, leftCubeData, rightCubeData };
unsigned int *cubeIndexData[FACE::FACES_SIZE]{ frontIndexBufferData, backIndexBufferData, frontIndexBufferData, backIndexBufferData, frontIndexBufferData, backIndexBufferData };

inline void setFace(int x, int y, int z, FloatVector fv[], Chunk &c, Face f, glm::vec3 chunkPosition)
{
	auto face = getBlockFace((c.blockData[x][y][z]), f);
	fv[f].push(glm::vec3(x, y, z) + chunkPosition);
	fv[f].push(face.x, face.y);
}

void CubeMeshRenderer::draw(Chunk **chunk, int size)
{
	for(int i=0; i<FACE::FACES_SIZE; i++)
	{
		positionData[i].size = 0;
	}

	for(int index =0; index < size; index ++)
	{
		Chunk &c = *chunk[index];
		glm::vec3 chunkPosition = c.getPositionInUnits();

		for (int x = 0; x < 16; x++)
		{
			for (int y = 0; y < BUILD_LIMIT; y++)
			{
				for (int z = 0; z < 16; z++)
				{
					if (c.blockData[x][y][z] != BLOCK::air)
					{
						//top
						if (y < BUILD_LIMIT - 1)
						{
							if (!isSolid(c.blockData[x][y + 1][z]))
							{
								setFace(x, y, z, positionData, c, FACE::top, chunkPosition);
							}
						}
						else
						{
							setFace(x, y, z, positionData, c, FACE::top, chunkPosition);
						}

						//bottom
						if (y > 0)
						{
							if (!isSolid(c.blockData[x][y - 1][z]))
							{
								setFace(x, y, z, positionData, c, FACE::bottom, chunkPosition);

							}
						}
						else
						{
							setFace(x, y, z, positionData, c, FACE::bottom, chunkPosition);

						}

						//front
						if (z < 15)
						{
							if (!isSolid(c.blockData[x][y][z + 1]))
							{
								setFace(x, y, z, positionData, c, FACE::front, chunkPosition);
							}
						}
						else
						{
							if(c.neighbours[CN::front])
							{
								if (!isSolid(c.neighbours[CN::front]->blockData[x][y][0]))
								{
									setFace(x, y, z, positionData, c, FACE::front, chunkPosition);
								}
							}
							else if (DRAW_EDGE_CHUNKS)
							{
								
									setFace(x, y, z, positionData, c, FACE::front, chunkPosition);
							}
						}

						//back
						if (z > 0)
						{
							if (!isSolid(c.blockData[x][y][z - 1]))
							{
								setFace(x, y, z, positionData, c, FACE::back, chunkPosition);
							}
						}
						else
						{
							if (c.neighbours[CN::back])
							{
								if (!isSolid(c.neighbours[CN::back]->blockData[x][y][15]))
								{
									setFace(x, y, z, positionData, c, FACE::back, chunkPosition);
								}
							}else if(DRAW_EDGE_CHUNKS)
							{
								setFace(x, y, z, positionData, c, FACE::back, chunkPosition);
							}

						}

						//left
						if (x > 0)
						{
							if (!isSolid(c.blockData[x - 1][y][z]))
							{
								setFace(x, y, z, positionData, c, FACE::left, chunkPosition);
							}
						}
						else
						{
							if (c.neighbours[CN::left])
							{
								if (!isSolid(c.neighbours[CN::left]->blockData[15][y][z]))
								{
									setFace(x, y, z, positionData, c, FACE::left, chunkPosition);
								}
							}else if(DRAW_EDGE_CHUNKS)
							{
								setFace(x, y, z, positionData, c, FACE::left, chunkPosition);
							}

						}

						//right
						if (x < 15)
						{
							if (!isSolid(c.blockData[x + 1][y][z]))
							{
								setFace(x, y, z, positionData, c, FACE::right, chunkPosition);
							}
						}
						else
						{
							if (c.neighbours[CN::right])
							{
								if (!isSolid(c.neighbours[CN::right]->blockData[0][y][z]))
								{
									setFace(x, y, z, positionData, c, FACE::right, chunkPosition);
								}
							}else if(DRAW_EDGE_CHUNKS)
							{
								setFace(x, y, z, positionData, c, FACE::right, chunkPosition);
							}
						}
					}
				}
			}
		}
	}

	

	glm::mat4 m = camera->getProjectionViewMatrix();
	float mag = 1.f / (float)texture->subDivisions;


	for(int i=0; i<FACE::FACES_SIZE; i++)
	{
		glBindVertexArray(vertexArrays[i]);

		glNamedBufferData(positionsBuffer[i], positionData[i].size * sizeof(float), positionData[i].data, GL_STREAM_DRAW);

		glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, &m[0][0]);
		glUniform1i(textureUniformLocation, 0);
		glUniform1f(magnifierUniformLocation, mag);

		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, positionData[i].size/5);
	}
	
	glBindVertexArray(0);
}

void CubeMeshRenderer::cleanup()
{
}

void CubeMeshRenderer::create()
{
	textureUniformLocation = sp->getUniformLocation("u_texture");
	matUniformLocation = sp->getUniformLocation("u_mat");
	magnifierUniformLocation = sp->getUniformLocation("u_magnifier");

	glCreateBuffers(FACE::FACES_SIZE, facesBuffer);
	glCreateBuffers(FACE::FACES_SIZE, facesIndexBuffer);
	glCreateBuffers(FACE::FACES_SIZE, positionsBuffer);
	glGenVertexArrays(FACE::FACES_SIZE, vertexArrays);

	for(int i=0; i<FACE::FACES_SIZE; i++)
	{
		glNamedBufferData(facesBuffer[i], sizeof(frontCubeData), cubeData[i], GL_STATIC_DRAW);
		glNamedBufferData(facesIndexBuffer[i], sizeof(frontIndexBufferData), cubeIndexData[i], GL_STATIC_DRAW);

		glBindVertexArray(vertexArrays[i]);
		glBindBuffer(GL_ARRAY_BUFFER, facesBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesIndexBuffer[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(float) * 5, 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(float) * 5, (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer[i]);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, 0, sizeof(float) * 5, 0);
		glVertexAttribDivisor(2, 1);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, 0, sizeof(float) * 5, (void*)(sizeof(float) * 3));
		glVertexAttribDivisor(3, 1);

		texture->bind(0);
		sp->bind();
		//glUniform1i(textureUniformLocation, 0);

		positionData[i].reserve(5 * 1000);
	}

	glBindVertexArray(0);

}
