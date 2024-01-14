#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

#include "Camera.h"
#include "../Collisions/AABB.h"

struct Vertex
{
	struct
	{
		float x, y, z;
	} m_pos;

	struct
	{
		float x, y;
	} m_texCoord;
};

struct RenderDesc
{
	Camera* m_camera;
	Shader* m_shader;
	Texture* m_texture;
	float m_ratio;
	bool m_isSelected;
};

class Quad
{
public:
	Quad() = default;
	Quad(glm::vec2 pos, glm::vec2 scale, float depth, unsigned int shaderIndex, unsigned int textureIndex);

	void Init(glm::vec2 pos, glm::vec2 scale, float depth, unsigned int shaderIndex, unsigned int textureIndex);
	void Cleanup();

	void Draw(RenderDesc& desc);

	void UpdateAABB();

	void SetTextureIndex(int index) { m_textureIndex = index; }
	void SetShaderIndex(int index) { m_shaderIndex= index; }
	void SetPos(glm::vec2 pos) { m_pos = pos; }
	void SetRotation(float rot) { m_rotation = rot; }
	void SetScale(glm::vec2 scale) { m_scale = scale; }
	void ChangePos(glm::vec2 pos) { m_pos += pos; }
	void ChangeRotation(float rot) { m_rotation += rot; }
	void ChangeScale(glm::vec2 scale) { m_scale += scale; }
	void SetDepth(float depth) { m_depth = depth; }
	void SetVisibility(bool visible) { m_visible = visible; }

	int getShaderIndex() { return m_shaderIndex; }
	int GetTextureIndex() { return m_textureIndex; }
	glm::vec2& GetPos() { return m_pos; }
	float GetDepth() { return m_depth; }
	glm::vec2& GetScale() { return m_scale; }
	float GetRotation() { return m_rotation; }
	AABB& GetAABB() { return m_aabb; }
	const bool& GetVisible() { return m_visible; }
private:
	unsigned int m_vao = 0, m_vbo = 0, m_ebo = 0;
	unsigned int m_shaderIndex = 0;
	unsigned int m_textureIndex = 0;

	glm::vec2 m_pos{};
	glm::vec2 m_scale{};
	float m_rotation = 0.0f;

	float m_depth = 0.0f;

	AABB m_aabb;

	bool m_visible = true;

	constexpr static Vertex m_vertices[4] = {
		{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
		{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
	};

	constexpr static unsigned int m_indices[6] = {
		0, 2, 1,
		1, 2, 3,
	};
};