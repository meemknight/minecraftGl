#include "debugTools.h"

static constexpr float r = 0.501;

float cubePositions[] =
{
	-1.f * r, 1.f * r, 1.f * r,
	-1.f * r, -1.f * r, 1.f * r,
	1.f * r, -1.f * r, 1.f * r,
	1.f * r, 1.f * r, 1.f * r,
	//
	-1.f * r, 1.f * r, -1.f * r,
	-1.f * r, -1.f * r, -1.f * r,
	1.f * r, -1.f * r, -1.f * r,
	1.f * r, 1.f * r, -1.f * r,
};

static constexpr unsigned int cubeIndexData[] =
{
	0,1, 1,2, 2,3, 3,0,
	4,5, 5,6, 6,7, 7,4,
	0,4, 1,5, 2,6, 3,7,
};

void CubeWireRenderer::addCube(glm::vec3 pos, glm::vec4 color)
{
	dataToDraw.push(pos);
	dataToDraw.push(color);
}

void CubeWireRenderer::setShader(ShaderProgram * sp)
{
	this->sp = sp;
	u_mat = glGetUniformLocation(sp->id, "u_mat");
}

void CubeWireRenderer::draw()
{

	glBindVertexArray(vertexArray);
	sp->bind();

	glNamedBufferData(perGeometryAttributes, dataToDraw.size * sizeof(float), dataToDraw.data, GL_STREAM_DRAW);

	glm::mat4 m = c->getProjectionViewMatrix();
	glUniformMatrix4fv(u_mat, 1, GL_FALSE, &m[0][0]);

	glDrawElementsInstanced(GL_LINES, 4 * 3 * 2, GL_UNSIGNED_INT, nullptr, dataToDraw.size / 7);
	dataToDraw.size = 0;
	glBindVertexArray(0);
}

void CubeWireRenderer::init(ShaderProgram * sp, FirstPersonCamera * c)
{
	this->c = c;

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	
	sp->bind();
	setShader(sp);

	glGenBuffers(1, &cubeId);
	glGenBuffers(1, &indexBufferId);
	glGenBuffers(1, &perGeometryAttributes);

	//shape
	glBindBuffer(GL_ARRAY_BUFFER, cubeId);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubePositions), cubePositions, GL_STATIC_DRAW);
	
	//pos, color
	glBindBuffer(GL_ARRAY_BUFFER, perGeometryAttributes);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3*sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndexData), cubeIndexData, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void cleanup()
{

}
