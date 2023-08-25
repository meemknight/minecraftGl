#pragma once
#include <glad/glad.h>
#include <glm\mat4x4.hpp>
#include "Shader.h"

struct SkyBox
{
	GLuint vertexArray = 0;
	GLuint vertexBuffer = 0;

	void createGpuData();
	void loadTexture(const char *names[6]);
	void loadTexture(const char *name);
	void clearGpuData();
	void draw(const glm::mat4 &viewProjMat);

	void bindCubeMap();

	ShaderProgram shader;
	GLuint texture;

};

