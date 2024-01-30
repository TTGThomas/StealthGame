#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Texture::Texture(const char* filePath)
{
	Init(filePath);
}

void Texture::Init(const char* filePath)
{
	glGenTextures(1, &m_id);

	int nChannels = 0;
	unsigned char* data = stbi_load(filePath, &m_width, &m_height, &nChannels, 4);

	GLenum type = GL_RGB;
	if (nChannels == 1)
		type = GL_RED;
	else if (nChannels == 3)
		type = GL_RGB;
	else if (nChannels == 4)
		type = GL_RGBA;

	if (data == nullptr)
		std::cout << "fail to load file: " << filePath << std::endl;

	glBindTexture(GL_TEXTURE_2D, m_id);

	glTexImage2D(GL_TEXTURE_2D, 0, type, m_width, m_height, 0, type, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
}

void Texture::Init(unsigned char* data, int width, int height, GLenum type)
{
	glGenTextures(1, &m_id);

	m_width = width;
	m_height = height;

	glBindTexture(GL_TEXTURE_2D, m_id);

	glTexImage2D(GL_TEXTURE_2D, 0, type, m_width, m_height, 0, type, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
}

void Texture::Cleanup()
{
	glDeleteTextures(1, &m_id);
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Activate(int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
}

unsigned char* Texture::GetDataFromFile(const char* filePath, int* x, int* y, int* nChannels)
{
	return stbi_load(filePath, x, y, nChannels, 4);
}