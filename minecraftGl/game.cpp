#include "game.h"

#include <GL/glew.h>
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

static GLuint quadBuff;
static GLuint indexBuffer;
ShaderProgram sp;

float quadData[] =
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


unsigned int indexBufferData[] =
{
	0,
	1,
	3,
	3,
	1,
	2,
};

Camera camera;
Texture bloc;

int width;
int height;

int initGame()
{
	glGenBuffers(1, &quadBuff);
	glBindBuffer(GL_ARRAY_BUFFER, quadBuff);
	glNamedBufferData(quadBuff, sizeof(quadData), quadData, GL_STATIC_DRAW);

	glCreateBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glNamedBufferData(indexBuffer, sizeof(indexBufferData), indexBufferData, GL_STATIC_DRAW);

	sp = ShaderProgram("vertex.vert", "fragment.frag");
	sp.bind();

	bloc.create("textures/block.png");

	camera.projectionData.width = &width;
	camera.projectionData.height = &height;

	return 1;
}

int gameLogic(float deltaTime)
{
	width = getWindowSizeX();
	height = getWindowSizeY();
	glClear(GL_COLOR_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, quadBuff);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(float) * 4, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	bloc.bind(0);
	sp.bind();
	sp.uniformi("u_texture", 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	glViewport(0, 0, getWindowSizeX(), getWindowSizeY());

	//camera.mouseUpdate(getRelMousePosition());

	GLint mat = sp.getUniformLocation("u_mat");
	glm::mat4 m = camera.getProjectionViewMatrix();
	glUniformMatrix4fv(mat, 1, GL_FALSE, &m[0][0]);

	return 1;
}
