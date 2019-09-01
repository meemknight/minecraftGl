#include "CubeMeshRenderer.h"

float frontQuadData[] =
{
	-0.8f, 0.8f,//pos
	 0.0f, 1.0f,//texcoord

	-0.8f, -0.8f,
	 0.0f,  0.0f,

	 0.8f, -0.8f,
	 1.0f,  0.0f,

	 0.8f, 0.8f,
	 1.0f, 1.0f,
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

CubeMeshRenderer::CubeMeshRenderer()
{
}

void CubeMeshRenderer::draw()
{
	glBindVertexArray(vertexArrays[FACES::front]);

	glm::mat4 m = camera->getProjectionViewMatrix();
	glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, &m[0][0]);
	glUniform1i(textureUniformLocation, 0);
	glUniform2f(indexUniformLocation, 3.f, 15.f);
	glUniform1f(magnifierUniformLocation, 1.f / (float)texture->subDivisions);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

	glCreateBuffers(FACES::FACES_SIZE, facesBuffer);
	glCreateBuffers(FACES::FACES_SIZE, facesIndexBuffer);
	glGenVertexArrays(FACES::FACES_SIZE, vertexArrays);

	glNamedBufferData(facesBuffer[FACES::front], sizeof(frontQuadData), frontQuadData, GL_STATIC_DRAW);
	glNamedBufferData(facesIndexBuffer[FACES::front], sizeof(frontIndexBufferData), frontIndexBufferData, GL_STATIC_DRAW);
	
	glBindVertexArray(vertexArrays[FACES::front]);
	glBindBuffer(GL_ARRAY_BUFFER, facesBuffer[FACES::front]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesIndexBuffer[FACES::front]);
	glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(float) * 4, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	texture->bind(0);
	sp->bind();

	//glUniform1i(textureUniformLocation, 0);
	glBindVertexArray(0);

}
