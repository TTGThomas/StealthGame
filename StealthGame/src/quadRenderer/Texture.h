#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture
{
public:
	Texture(const char* filePath);

	void Bind();
	void Unbind();

	void Activate(int index);
private:
	unsigned int m_id = 0;
	int m_width = 0, m_height = 0;
};