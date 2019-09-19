#include "CubeMeshRenderer.h"
constexpr float r = 0.5f;


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


void CubeMeshRenderer::draw(Chunk **chunk, int size)
{
	
	glm::mat4 m = camera->getProjectionViewMatrix();
	float mag = 1.f / (float)texture->subDivisions;

	for(int index=0; index<size;index++)
	{
		Chunk &c = *chunk[index];

		for (int i = 0; i < FACE::FACES_SIZE; i++)
		{
			glBindVertexArray(vertexArrays[i]);

			glNamedBufferData(positionsBuffer[i], c.positionData[i].size * sizeof(float), c.positionData[i].data, GL_STREAM_DRAW);

			glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, &m[0][0]);
			glUniform1i(textureUniformLocation, 0);
			glUniform1f(magnifierUniformLocation, mag);

			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, c.positionData[i].size / 5);
		}
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
