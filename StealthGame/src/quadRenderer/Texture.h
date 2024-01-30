#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../UUID.h"

class Texture
{
public:
	Texture() = default;
	Texture(const char* filePath);

	void Init(const char* filePath);
	void Init(unsigned char* data, int width, int height, GLenum type = GL_RGBA);

	void Cleanup();

	void Bind();
	void Unbind();

	void Activate(int index);

	UUID& GetUUID() { return m_uuid; }
private:
	unsigned int m_id = 0;
	int m_width = 0, m_height = 0;
	UUID m_uuid;
};