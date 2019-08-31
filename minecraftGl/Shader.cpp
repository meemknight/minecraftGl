/////////////////////////////////////////////
//Shader.cpp
//Copyright(c) 2019 Luta Vlad
//https://github.com/meemknight/OpenGLEngine
/////////////////////////////////////////////
#include "Shader.h"
#include "tools.h"
#include <cstdio>
#include <iostream>

ShaderProgram::ShaderProgram()
{

}

ShaderProgram::ShaderProgram(const VertexShader & vs, const FragmentShader & fs) :
	vs(vs), fs(fs)
{
	compileProgram();
}

//todo delete parts of the program
void ShaderProgram::compileProgram()
{
	id = glCreateProgram();
	glAttachShader(id, vs.id);
	glAttachShader(id, fs.id);
	glLinkProgram(id);

	int info;
	glGetProgramiv(id, GL_LINK_STATUS, &info);
	if (info != GL_TRUE)
	{
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &info);
		char *p = new char[info];

		glGetProgramInfoLog(id, info, 0, p);
		elog("Error compiling Shader:", p);
		delete[] p;
	}

	glValidateProgram(id);



}

void ShaderProgram::bind()
{
	glUseProgram(id);
}

void ShaderProgram::unBind()
{
	glUseProgram(0);
}

void ShaderProgram::deleteProgram()
{
	glDeleteProgram(id);
	locations.clear();
	subRoutines.clear();
	id = 0;
}


int ShaderProgram::getUniformLocation(const char* name)
{
	if (locations.find(name) == locations.end())
	{
		int a = glGetUniformLocation(id, name);

		if (a == -1)
		{
			elog("Uniform error: ", name);
		}

		//llog("Loaded Uniform: ", name);
		locations[name] = a;
		return a;
	}
	else
	{
		return locations[name];
	}

}

unsigned int ShaderProgram::getSoubRutineLocation(const char * name, GLenum type)
{
	if (subRoutines.find(name) == subRoutines.end())
	{
		unsigned int a = glGetSubroutineIndex(id, type, name);

		if (a == GL_INVALID_INDEX)
		{
			llog("Uniform subroutine error: ", name);
		}

		subRoutines[name] = a;
		return a;
	}
	else
	{
		return subRoutines[name];
	}
}

void ShaderProgram::uniform(const char * name, float a)
{
	bind(); // ? usefull
	glUniform1f(getUniformLocation(name), a);
}

void ShaderProgram::uniformi(const char * name, int a)
{
	bind(); // ? usefull
	glUniform1i(getUniformLocation(name), a);
}

void ShaderProgram::uniform(const char * name, float a, float b, float c, float d)
{
	bind(); // ? usefull
	glUniform4f(getUniformLocation(name), a, b, c, d);
}


void ShaderProgram::uniform(const char * name, float a, float b, float c)
{
	bind(); // ? usefull
	glUniform3f(getUniformLocation(name), a, b, c);
}

void ShaderProgram::uniform(const char * name, int count, float * a)
{
	bind(); // ? usefull
	glUniform1fv(getUniformLocation(name), count, a);
}
