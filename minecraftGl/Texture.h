/////////////////////////////////////////////
//Texture.h
//Copyright(c) 2019 Luta Vlad
//https://github.com/meemknight/OpenGLEngine
/////////////////////////////////////////////

#pragma once
#include<GL/glew.h>

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