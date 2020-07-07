#include "CubeMeshRenderer.h"
constexpr float r = 0.5f;


#pragma region cubeData

const float frontCubeData[] =
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

const float backCubeData[] =
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

const float topCubeData[] =
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

const float bottomCubeData[] =
{
		-1.0f*r, -1.0f*r, -1.0*r,
		1.0f, 0.0f,

		-1.0f*r, -1.0f*r, 1.0*r,
		1.0f, 1.0f,

		1.0f*r, -1.0f*r, 1.0*r,
		0.0f, 1.0f,

		1.0f*r, -1.0f*r, -1.0*r,
		0.0f, 0.0f,
};

const float leftCubeData[] =
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

const float rightCubeData[] =
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

const unsigned int frontIndexBufferData[] =
{
	0,
	1,
	3,
	3,
	1,
	2,
};

const unsigned int backIndexBufferData[] =
{
	3,
	1,
	0,
	3,
	2,
	1,
};

const float herbsData[] =
{
	-1.0f*r, 1.0f*r, 1.0*r,//pos
	 0.0f, 1.0f,//texcoord

	-1.0f*r, -1.0f*r, 1.0*r,
	 0.0f,  0.0f,

	 1.0f*r, -1.0f*r, -1.0*r,
	 1.0f,  0.0f,

	 1.0f*r, 1.0f*r, -1.0*r,
	 1.0f, 1.0f,
	 // //todo flip texcoords v
	-1.0f*r, 1.0f*r, -1.0*r,//pos
	 0.0f, 1.0f,//texcoord

	-1.0f*r, -1.0f*r, -1.0*r,
	 0.0f,  0.0f,

	 1.0f*r, -1.0f*r, 1.0*r,
	 1.0f,  0.0f,

	 1.0f*r, 1.0f*r, 1.0*r,
	 1.0f, 1.0f,
};

const unsigned int herbsIndexBufferData[] =
{
	0,
	1,
	3,
	3,
	1,
	2,
	//
	0+4,
	1+4,
	3+4,
	3+4,
	1+4,
	2+4,
};

#pragma endregion

const float ambienceData[FACE::FACES_SIZE] = 
{
	0.85f, 0.85f, 1.f, 0.65f, 0.85f, 0.85f,
};

const float *cubeData[FACE::FACES_SIZE]{ frontCubeData, backCubeData, topCubeData, bottomCubeData, leftCubeData, rightCubeData };
const unsigned int *cubeIndexData[FACE::FACES_SIZE]{ frontIndexBufferData, backIndexBufferData, frontIndexBufferData, backIndexBufferData, frontIndexBufferData, backIndexBufferData };


void CubeMeshRenderer::draw(Chunk **chunk, int size)
{
	assert(sp, "Error, no shader");
	sp->bind();

	assert(texture, "Error, no texture");
	texture->bind();

	glm::mat4 m = camera->getProjectionViewMatrix();
	float mag = 1.f / (float)texture->subDivisions;

	glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, &m[0][0]);
	glUniform1i(textureUniformLocation, 0);
	glUniform1f(magnifierUniformLocation, mag);
	glUniform1f(distanceUniformLocation, camera->farPlane);

#pragma region cull unseen faces

	int mask[FACE::FACES_SIZE] = { 1,1,1,1,1,1 };
	
	{
		float margin = 0.8;

		if (camera->viewDirection.x > margin)
		{
			mask[FACE::right] = 0;
		}
		else if (camera->viewDirection.x < -margin)
		{
			mask[FACE::left] = 0;
		}

		if (camera->viewDirection.y > margin)
		{
			mask[FACE::top] = 0;
		}
		else if (camera->viewDirection.y < -margin)
		{
			mask[FACE::bottom] = 0;
		}

		if (camera->viewDirection.z > margin)
		{
			mask[FACE::front] = 0;
		}
		else if (camera->viewDirection.z < -margin)
		{
			mask[FACE::back] = 0;
		}
	}

#pragma endregion

	int sizes[FACE::FACES_SIZE] = {};
	int herbsSize = 0;
	for (int index = 0; index < size; index++)
	{
			Chunk &c = *chunk[index];

			//if(c.shouldRecreate || !c.chunkBuilt)
			//{
			//	continue;
			//}

			for (int i = 0; i < FACE::FACES_SIZE; i++)
			{
				if (mask[i])
				{
					sizes[i] += c.positionData[i].size;
				}
			}

			herbsSize += c.positionData[6].size;
	}
	
	for(int i=0; i< FACE::FACES_SIZE; i++)
	{
		if(mask[i])
		{
			sizes[i] += additionalBlocks.size() * 5;
		}
	}

	for (int i = 0; i < FACE::FACES_SIZE; i++)
	{
		if (mask[i])
		{
			glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, sizes[i] * sizeof(float), nullptr, GL_STREAM_DRAW);
			int pos = 0;

			for (int index = 0; index < additionalBlocks.size(); index++)
			{
				glm::vec3 &p = additionalBlocks[index].pos;
				float f[5];
				f[0] = p.x;
				f[1] = p.y;
				f[2] = p.z;
				BlockFace face = getBlockFace(additionalBlocks[index].b, i);
				f[3] = face.x;
				f[4] = face.y;

				glBufferSubData(GL_ARRAY_BUFFER, pos * sizeof(float), 5 * sizeof(float), f);
				pos += 5;
			}

			for (int index = 0; index < size; index++)
			{
				Chunk &c = *chunk[index];

				//if (c.shouldRecreate || !c.chunkBuilt)
				//{
				//	continue;
				//}

				glBufferSubData(GL_ARRAY_BUFFER, pos * sizeof(float), c.positionData[i].size * sizeof(float), c.positionData[i].data);
				pos += c.positionData[i].size;
			}

			glBindVertexArray(vertexArrays[i]);
			glUniform1f(ambienceUniformLocation, ambienceData[i]);
			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, sizes[i] / 5);
		}
	}
	
	{
		//herbs
		int pos = 0;

		glBindBuffer(GL_ARRAY_BUFFER, herbsPositionData);
		glBufferData(GL_ARRAY_BUFFER, herbsSize * sizeof(float), nullptr, GL_STREAM_DRAW);

		for (int index = 0; index < size; index++)
		{
			Chunk &c = *chunk[index];

			//if (c.shouldRecreate || !c.chunkBuilt)
			//{
			//	continue;
			//}

			glBufferSubData(GL_ARRAY_BUFFER, pos * sizeof(float), c.positionData[6].size * sizeof(float), c.positionData[6].data);
			pos += c.positionData[6].size;
		}

		glDisable(GL_CULL_FACE);
		glBindVertexArray(herbsVertexArray);
		glUniform1f(ambienceUniformLocation, ambienceData[0]);
		glDrawElementsInstanced(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0, pos / 5);
		glEnable(GL_CULL_FACE);
	}

	additionalBlocks.clear();
	
	glBindVertexArray(0);
}

void CubeMeshRenderer::cleanup()
{
	//todo
}

void CubeMeshRenderer::create()
{
	additionalBlocks.reserve(1);

	textureUniformLocation = sp->getUniformLocation("u_texture");
	matUniformLocation = sp->getUniformLocation("u_mat");
	magnifierUniformLocation = sp->getUniformLocation("u_magnifier");
	ambienceUniformLocation = sp->getUniformLocation("u_ambience");
	distanceUniformLocation = sp->getUniformLocation("u_distance");

	glGenBuffers(FACE::FACES_SIZE, facesBuffer);
	glGenBuffers(FACE::FACES_SIZE, facesIndexBuffer);
	glGenBuffers(FACE::FACES_SIZE, positionsBuffer);
	glGenVertexArrays(FACE::FACES_SIZE, vertexArrays);

	glGenBuffers(1, &herbsShapeData);
	glGenBuffers(1, &herbsShapeIndexBuffer);
	glGenBuffers(1, &herbsPositionData);
	glGenVertexArrays(1, &herbsVertexArray);

	for(int i=0; i<FACE::FACES_SIZE; i++)
	{
		glBindVertexArray(vertexArrays[i]);
		glBindBuffer(GL_ARRAY_BUFFER, facesBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesIndexBuffer[i]);

		glBufferData(GL_ARRAY_BUFFER, sizeof(frontCubeData), cubeData[i], GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frontIndexBufferData), cubeIndexData[i], GL_STATIC_DRAW);

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

	positionData[6].reserve(5 * 10);

	//herbs
	{
		glBindVertexArray(herbsVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, herbsShapeData);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, herbsShapeIndexBuffer);

		glBufferData(GL_ARRAY_BUFFER, sizeof(herbsData), herbsData, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(herbsIndexBufferData), herbsIndexBufferData, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(float) * 5, 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(float) * 5, (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, herbsPositionData);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, 0, sizeof(float) * 5, 0);
		glVertexAttribDivisor(2, 1);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, 0, sizeof(float) * 5, (void*)(sizeof(float) * 3));
		glVertexAttribDivisor(3, 1);

		texture->bind(0);
		sp->bind();

	}

	glBindVertexArray(0);

}

void CubeMeshRenderer::addSingleCube(int x, int y, int z, Block type)
{
	additionalBlocks.push_back({ { x,y,z }, type });
}
