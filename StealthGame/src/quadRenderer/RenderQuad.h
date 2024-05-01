#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

#include "Camera.h"
#include "../Collisions/AABB.h"

#include "../UUID.h"

struct RenderDesc
{
	Camera* m_camera;
	Shader* m_shader;
	Texture* m_texture;
	float m_ratio;
	bool m_isSelected;
	bool m_useTexture;
};

struct RenderQuadInitDesc
{
	bool m_followCameraOffset = true;
	float m_depth;
	uint64_t m_shaderUUID;
	uint64_t m_textureUUID;
};

class RenderQuad
{
public:
	RenderQuad() = default;
	RenderQuad(RenderQuadInitDesc& desc, GameUUID uuid);

	void Init(float depth, uint64_t shaderUUID, uint64_t textureUUID);
	void Cleanup();

	void Draw(RenderDesc& desc);
	glm::mat4 Matrix(Camera* camera);

	void UpdateRenderQuad(class Scene* scene);
	void UpdateRenderQuad(Scene* scene, GameUUID uuid);

	void SetTextureUUID(uint64_t uuid) { m_textureUUID = uuid; }
	void SetShaderUUID(uint64_t uuid) { m_shaderUUID = uuid; }
	void SetDepth(float depth) { m_depth = depth; }
	void SetVisibility(bool visible) { m_visible = visible; }
	void SetFollowCamera(bool follow) { m_followCamera = follow; }
	void SetAlpha(float alpha) { m_alpha = alpha; }
	void SetSideFrames(int sideFrames) { m_sideFrames = sideFrames; }
	void SetFrameIndex(float frameIndex) { m_frameIndex = frameIndex; }

	uint64_t getShaderUUID() { return m_shaderUUID; }
	uint64_t GetTextureUUID() { return m_textureUUID;; }
	float GetDepth() { return m_depth; }
	float GetAlpha() { return m_alpha; }
	bool GetVisible() { return m_visible; }
	bool GetFollowCamera() { return m_followCamera; }
	int GetSideFrames() { return m_sideFrames; }
	float GetFrameIndex() { return m_frameIndex; }
private:
	unsigned int m_vao = 0, m_vbo = 0, m_ebo = 0;
	uint64_t m_shaderUUID = 0;
	uint64_t m_textureUUID = 0;

	int m_screenRatioLoc = -1;
	int m_matrixLoc = -1;
	int m_alphaLoc = -1;
	int m_depthLoc = -1;
	int m_frameIndexLoc = -1;
	int m_sideFramesLoc = -1;
	int m_useTextureLoc = -1;

	float m_frameIndex = 0;
	int m_sideFrames = 1;

	glm::vec2 m_pos{};
	glm::vec2 m_radius{};
	float m_rotation = 0.0f;

	GameUUID m_uuid;

	// if alpha is -1, it will use the texture alpha
	float m_alpha = -1.0f;
	float m_depth = 0.0f;

	bool m_visible = true;
	bool m_followCamera = true;

	//constexpr static Vertex m_vertices[4] = {
	//	{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
	//	{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
	//	{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
	//	{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
	//};
	//
	//constexpr static unsigned int m_indices[6] = {
	//	0, 2, 1,
	//	1, 2, 3,
	//};
};