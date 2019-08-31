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

	Texture();
	Texture(const char *name);
	void create(const char *name);
	void bind(unsigned int sample = 0);
	void unBind();
	void cleanup();
};