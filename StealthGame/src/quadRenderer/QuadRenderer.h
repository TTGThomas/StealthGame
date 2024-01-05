#pragma once

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Quad.h"
#include "Shader.h"
#include "Texture.h"

class QuadRenderer
{
public:
	void AddQuad(glm::vec2 pos, glm::vec2 scale, unsigned int shaderIndex, unsigned int textureIndex);
	void AddShader(const char* vertex, const char* fragment);
	void AddTexture(const char* texturePath);

	void Render();

	std::vector<Quad>& GetQuads() { return m_quads; }
private:
	std::vector<Quad> m_quads;
	std::vector<Shader> m_shaders;
	std::vector<Texture> m_textures;
};