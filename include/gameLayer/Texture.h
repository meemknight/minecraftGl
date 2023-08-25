/////////////////////////////////////////////
//Texture.h
//Copyright(c) 2019 Luta Vlad
//https://github.com/meemknight/OpenGLEngine
/////////////////////////////////////////////

#pragma once

#include<glad/glad.h>
#include<glm/vec4.hpp>

struct Texture
{
	GLuint id = 0;

	int width = 0;
	int heigth = 0;

	//this reprezents the number of blocks per row and collom
	int subDivisions = 1;

	Texture();
	Texture(const char *name);
	void create(const char *name);
	void bind(unsigned int sample = 0);
	void unBind();
	void cleanup();
};

glm::vec4 computeTextureAtlas(int xCount, int yCount, int x, int y);

struct TextureAtlas
{
	int xCount;
	int yCount;

	glm::vec4 get(int x, int y)
	{
		return computeTextureAtlas(xCount, yCount, x, y);
	}
};