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
	bool m_followCamera = true;
	float m_depth;
	uint64_t m_shaderUUID;
	uint64_t m_textureUUID;
};

class RenderQuad
{
public:
	RenderQuad() = default;
	RenderQuad(RenderQuadInitDesc& desc, UUID uuid);

	void Init(float depth, uint64_t shaderUUID, uint64_t textureUUID);
	void Cleanup();

	void Draw(RenderDesc& desc);

	void UpdateRenderQuad(class Scene* scene);
	void UpdateRenderQuad(Scene* scene, UUID uuid);

	void SetTextureUUID(uint64_t uuid) { m_textureUUID = uuid; }
	void SetShaderUUID(uint64_t uuid) { m_shaderUUID = uuid; }
	void SetDepth(float depth) { m_depth = depth; }
	void SetVisibility(bool visible) { m_visible = visible; }
	void SetFollowCamera(bool follow) { m_followCamera = follow; }

	uint64_t getShaderUUID() { return m_shaderUUID; }
	uint64_t GetTextureUUID() { return m_textureUUID;; }
	float GetDepth() { return m_depth; }
	bool GetVisible() { return m_visible; }
	bool GetFollowCamera() { return m_followCamera; }
private:
	unsigned int m_vao = 0, m_vbo = 0, m_ebo = 0;
	uint64_t m_shaderUUID = 0;
	uint64_t m_textureUUID = 0;

	glm::vec2 m_pos{};
	glm::vec2 m_radius{};
	float m_rotation = 0.0f;

	UUID m_uuid;

	float m_depth = 0.0f;

	bool m_visible = true;
	bool m_followCamera = true;

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