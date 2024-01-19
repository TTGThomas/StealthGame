#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

#include "Camera.h"
#include "../Collisions/AABB.h"

#include "../UUID.h"

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

struct RenderQuadInitDesc
{
	float m_depth;
	unsigned int m_shaderIndex;
	unsigned int m_textureIndex;
};

class RenderQuad
{
public:
	RenderQuad() = default;
	RenderQuad(RenderQuadInitDesc& desc);

	void Init(float depth, unsigned int shaderIndex, unsigned int textureIndex);
	void Cleanup();

	void Draw(RenderDesc& desc);

	void UpdateRenderQuad(class Scene* scene);
	void UpdateRenderQuad(Scene* scene, UUID uuid);

	void SetTextureIndex(int index) { m_textureIndex = index; }
	void SetShaderIndex(int index) { m_shaderIndex= index; }
	void SetDepth(float depth) { m_depth = depth; }
	void SetVisibility(bool visible) { m_visible = visible; }

	int getShaderIndex() { return m_shaderIndex; }
	int GetTextureIndex() { return m_textureIndex; }
	float GetDepth() { return m_depth; }
	const bool& GetVisible() { return m_visible; }
private:
	unsigned int m_vao = 0, m_vbo = 0, m_ebo = 0;
	unsigned int m_shaderIndex = 0;
	unsigned int m_textureIndex = 0;

	glm::vec2 m_pos{};
	glm::vec2 m_radius{};
	float m_rotation = 0.0f;

	UUID m_uuid;

	float m_depth = 0.0f;

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