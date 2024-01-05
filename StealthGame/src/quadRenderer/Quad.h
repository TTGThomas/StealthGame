#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

#include "Camera.h"

struct Vertex
{
	struct
	{
		float x, y;
	} m_pos;

	struct
	{
		float x, y;
	} m_texCoord;
};

class Quad
{
public:
	Quad() = default;
	Quad(glm::vec2 pos, glm::vec2 scale, unsigned int shaderIndex, unsigned int textureIndex);
	~Quad();

	void Init(glm::vec2 pos, glm::vec2 scale, unsigned int shaderIndex, unsigned int textureIndex);

	void Draw(Camera* camera, Shader* shader, Texture* texture, float ratio);

	int getShaderIndex() { return m_shaderIndex; }
	int GetTextureIndex() { return m_textureIndex; }
	glm::vec2& GetPos() { return m_pos; }
	glm::vec2& GetScale() { return m_scale; }
private:
	unsigned int m_vao = 0, m_vbo = 0, m_ebo = 0;
	unsigned int m_shaderIndex = 0;
	unsigned int m_textureIndex = 0;

	glm::vec2 m_pos{};
	glm::vec2 m_scale{};

	constexpr static Vertex m_vertices[4] = {
		{{-1.0f,  1.0f}, {0.0f, 0.0f}},
		{{ 1.0f,  1.0f}, {1.0f, 0.0f}},
		{{-1.0f, -1.0f}, {0.0f, 1.0f}},
		{{ 1.0f, -1.0f}, {1.0f, 1.0f}},
	};

	constexpr static unsigned int m_indices[6] = {
		0, 2, 1,
		1, 2, 3,
	};
};