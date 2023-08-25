#include "SkyBox.h"
#include <stb_image/stb_image.h>

float skyboxVertices[] = {
// positions          
-1.0f,  1.0f, -1.0f,
-1.0f, -1.0f, -1.0f,
 1.0f, -1.0f, -1.0f,
 1.0f, -1.0f, -1.0f,
 1.0f,  1.0f, -1.0f,
-1.0f,  1.0f, -1.0f,

-1.0f, -1.0f,  1.0f,
-1.0f, -1.0f, -1.0f,
-1.0f,  1.0f, -1.0f,
-1.0f,  1.0f, -1.0f,
-1.0f,  1.0f,  1.0f,
-1.0f, -1.0f,  1.0f,

 1.0f, -1.0f, -1.0f,
 1.0f, -1.0f,  1.0f,
 1.0f,  1.0f,  1.0f,
 1.0f,  1.0f,  1.0f,
 1.0f,  1.0f, -1.0f,
 1.0f, -1.0f, -1.0f,

-1.0f, -1.0f,  1.0f,
-1.0f,  1.0f,  1.0f,
 1.0f,  1.0f,  1.0f,
 1.0f,  1.0f,  1.0f,
 1.0f, -1.0f,  1.0f,
-1.0f, -1.0f,  1.0f,

-1.0f,  1.0f, -1.0f,
 1.0f,  1.0f, -1.0f,
 1.0f,  1.0f,  1.0f,
 1.0f,  1.0f,  1.0f,
-1.0f,  1.0f,  1.0f,
-1.0f,  1.0f, -1.0f,

-1.0f, -1.0f, -1.0f,
-1.0f, -1.0f,  1.0f,
 1.0f, -1.0f, -1.0f,
 1.0f, -1.0f, -1.0f,
-1.0f, -1.0f,  1.0f,
 1.0f, -1.0f,  1.0f
};

void SkyBox::createGpuData()
{
	shader = ShaderProgram(RESOURCES_PATH "skyBox.vert", RESOURCES_PATH  "skyBox.frag");

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);


	glBindVertexArray(0);
}

void SkyBox::loadTexture(const char *names[6])
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int w, h, nrChannels;
	unsigned char *data;
	for (unsigned int i = 0; i < 6; i++)
	{
		stbi_set_flip_vertically_on_load(false);
		data = stbi_load(names[i], &w, &h, &nrChannels, 3);

		if (data)
		{

			//gausianBlurRGB(data, w, h, 10);

			glTexImage2D(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "err loading " << names[i] << "\n";
		}


	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



}

void SkyBox::loadTexture(const char *name)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int width, height, nrChannels;
	unsigned char *data;


	stbi_set_flip_vertically_on_load(false);
	data = stbi_load(name, &width, &height, &nrChannels, 3);

	//right
	//left
	//top
	//bottom
	//front
	//back

	auto getPixel = [&](int x, int y, unsigned char *data)
	{
		return data + 3 * (x + y * width);
	};

	glm::ivec2 paddings[6] =
	{
		{(width / 4) * 2, (height / 3) * 1, },
		{(width / 4) * 0, (height / 3) * 1, },
		{(width / 4) * 1, (height / 3) * 0, },
		{(width / 4) * 1, (height / 3) * 2, },
		{(width / 4) * 1, (height / 3) * 1, },
		{(width / 4) * 3, (height / 3) * 1, },
	};

	if (data)
	{
		for (unsigned int i = 0; i < 6; i++)
		{
			unsigned char *extractedData = new unsigned char[3 * (width / 4) * (height / 3)];

			int index = 0;

			int paddingX = paddings[i].x;
			int paddingY = paddings[i].y;

			for (int j = 0; j < height / 3; j++)
				for (int i = 0; i < width / 4; i++)
				{
					extractedData[index] = *getPixel(i + paddingX, j + paddingY, data);
					extractedData[index + 1] = *(getPixel(i + paddingX, j + paddingY, data) + 1);
					extractedData[index + 2] = *(getPixel(i + paddingX, j + paddingY, data) + 2);
					//extractedData[index] = 100;
					//extractedData[index + 1] = 100;
					//extractedData[index + 2] = 100;
					index += 3;
				}

			glTexImage2D(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0, GL_RGB, width / 4, height / 3, 0, GL_RGB, GL_UNSIGNED_BYTE, extractedData
			);



			delete[] extractedData;
		}

		stbi_image_free(data);

	}
	else
	{
		std::cout << "err loading " << name << "\n";
	}


	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


}

void SkyBox::clearGpuData()
{
}

void SkyBox::draw(const glm::mat4 &viewProjMat)
{
	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(vertexArray);

	bindCubeMap();

	shader.bind();

	glUniformMatrix4fv(shader.getUniformLocation("u_viewProjection"), 1, GL_FALSE, &viewProjMat[0][0]);
	glUniform1i(shader.getUniformLocation("u_skybox"), 1);

	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
	glDepthFunc(GL_LESS);

	glBindVertexArray(0);
}

void SkyBox::bindCubeMap()
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}
