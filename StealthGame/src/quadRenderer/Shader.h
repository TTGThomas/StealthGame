#pragma once

#include <fstream>
#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../UUID.h"

class Shader
{
public:
	Shader() = default;
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	~Shader();

	void Cleanup();

	void Init(const char* vertexShaderPath, const char* fragmentShaderPath);

	void Bind();
	void Unbind();

	unsigned int GetProgram() { return m_program; }
	GameUUID& GetUUID() { return m_uuid; }
private:
	unsigned int CompileShader(const char* path, GLenum type);
private:
	unsigned int m_program = 0;
	GameUUID m_uuid;
};

#define LOCATION(shader, name) glGetUniformLocation((shader).GetProgram(), (name))