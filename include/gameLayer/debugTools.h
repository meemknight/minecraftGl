#pragma once
#include "Camera.h"
#include <glad/glad.h>
#include "Shader.h"
#include "FloatVector.h"

struct CubeWireRenderer
{
	ShaderProgram *sp;
	FirstPersonCamera *c;

	GLuint cubeId;
	GLuint indexBufferId;
	GLuint perGeometryAttributes;
	GLuint vertexArray;

	GLint u_mat = -1;

	void addCube(glm::vec3 pos, glm::vec4 color);
	void setShader(ShaderProgram *sp);
	void cleanup();
	void draw();

	void init(ShaderProgram *sp, FirstPersonCamera *c);

	FloatVector dataToDraw;
};