#include "CubeMeshRenderer.h"

#pragma region cubeData

float frontCubeData[] =
{
	-1.0f, 1.0f, 1.0,//pos
	 0.0f, 1.0f,//texcoord

	-1.0f, -1.0f, 1.0,
	 0.0f,  0.0f,

	 1.0f, -1.0f, 1.0,
	 1.0f,  0.0f,

	 1.0f, 1.0f, 1.0,
	 1.0f, 1.0f,
};

float backCubeData[] = 
{
	-1.0f, 1.0f, -1.0,//pos
	 1.0f, 1.0f,//texcoord

	-1.0f, -1.0f, -1.0,
	 1.0f,  0.0f,

	 1.0f, -1.0f, -1.0,
	 0.0f,  0.0f,

	 1.0f, 1.0f, -1.0,
	 0.0f, 1.0f,
};

float topCubeData[] =
{
		-1.0f, 1.0f, -1.0,
		0.0f, 1.0f,

		-1.0f, 1.0f, 1.0,
		0.0f, 0.0f,

		1.0f, 1.0f, 1.0,
		1.0f, 0.0f,

		1.0f, 1.0f, -1.0,
		1.0f, 1.0f,
};

float bottomCubeData[] =
{
		-1.0f, -1.0f, -1.0,
		0.0f, 1.0f,

		-1.0f, -1.0f, 1.0,
		0.0f, 0.0f,

		1.0f, -1.0f, 1.0,
		1.0f, 0.0f,

		1.0f, -1.0f, -1.0,
		1.0f, 1.0f,
};

float leftCubeData[] = 
{
	-1.0f, 1.0f, -1.0,
	 0.0f, 1.0f,

	 -1.0f, -1.0f, -1.0,
	 0.0f, 0.0f,

	 -1.0f, -1.0f, 1.0,
	 1.0f, 0.0f,

	 -1.0f, 1.0f, 1.0,
	 1.0f, 1.0f,
};

float rightCubeData[] =
{
	1.0f, 1.0f, -1.0,
	 1.0f, 1.0f,

	 1.0f, -1.0f, -1.0,
	 1.0f, 0.0f,

	 1.0f, -1.0f, 1.0,
	 0.0f, 0.0f,

	 1.0f, 1.0f, 1.0,
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

void CubeMeshRenderer::draw()
{
	glm::mat4 m = camera->getProjectionViewMatrix();
	float mag = 1.f / (float)texture->subDivisions;

	for(int i=0; i<FACE::FACES_SIZE; i++)
	{
		auto face = getBlockFace(BLOCK::grass, i);
		glBindVertexArray(vertexArrays[i]);

		glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, &m[0][0]);
		glUniform1i(textureUniformLocation, 0);
		glUniform2f(indexUniformLocation, (float)face.x, (float)face.y);
		glUniform1f(magnifierUniformLocation, mag);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
	indexUniformLocation = sp->getUniformLocation("u_index");
	magnifierUniformLocation = sp->getUniformLocation("u_magnifier");

	glCreateBuffers(FACE::FACES_SIZE, facesBuffer);
	glCreateBuffers(FACE::FACES_SIZE, facesIndexBuffer);
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

		texture->bind(0);
		sp->bind();
		//glUniform1i(textureUniformLocation, 0);
	}

	glBindVertexArray(0);

}
