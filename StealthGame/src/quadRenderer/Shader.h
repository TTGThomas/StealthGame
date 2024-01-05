#pragma once

#include <fstream>
#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Shader
{
public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	~Shader();

	void Bind();
	void Unbind();

	unsigned int GetProgram() { return m_program; }
private:
	unsigned int CompileShader(const char* path, GLenum type);
private:
	unsigned int m_program;
};

#define LOCATION(shader, name) glGetUniformLocation((shader).GetProgram(), (name))