
/////////////////////////////////////////////
//Texture.cpp
//Copyright(c) 2019 Luta Vlad
//https://github.com/meemknight/OpenGLEngine
/////////////////////////////////////////////
#include "Texture.h"
#include "tools.h"
#include <iostream>
#include <Windows.h>
#include <string.h>
#include <stdint.h>

#include "stb_image.h"

Texture::Texture()
{
}

Texture::Texture(const char * name)
{
	create(name);
}

void Texture::create(const char * name)
{
	if (name == nullptr)
	{
		return;
	}

	stbi_set_flip_vertically_on_load(true);
	int x = 0, y = 0, chanels = 0;
	uint8_t *buff = stbi_load(name, &x, &y, &chanels, 4);

	if (!buff)
	{
		//todo check if working
		elog("error loading ", name);
		id = 0;
	}
	else
	{
		glog("Loaded: ", name);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_LINEAR, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_REPEAT, GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, buff);

		glGenerateMipmap(GL_TEXTURE_2D);

		//int NumberOfExtensions;
		//glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
		//for (int i = 0; i < NumberOfExtensions; i++)
		//{
		//	const char *ccc = (const char*)glGetStringi(GL_EXTENSIONS, i);
		//	if (strcmp(ccc, "filter anisotropic") == 0)
		//	{
		//		float amount;
		//		amount = 4;
		//		//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
		//
		//		break;
		//	}
		//}
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

		stbi_image_free(buff);
	}


}

void Texture::bind(unsigned int sample)
{
	glActiveTexture(GL_TEXTURE0 + sample);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unBind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::cleanup()
{
	if(id)
	{
		glDeleteTextures(1, &id);
		id = 0;
	}
}
