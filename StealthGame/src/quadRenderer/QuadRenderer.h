#pragma once

#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include "RenderQuad.h"
#include "Shader.h"
#include "Texture.h"

#include "Camera.h"

#include "../UUID.h"
#include "../WildException.h"

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

struct InstanceData
{
	glm::vec4 m_matrix0;
	glm::vec4 m_matrix1;
	glm::vec4 m_matrix2;
	glm::vec4 m_matrix3;
	glm::vec2 m_texDimension;
	float m_texIndex;
	float m_alpha;
	float m_depth;
	float m_sideFrames;
	float m_frameIndex;
};

class QuadRenderer
{
public:
	QuadRenderer(class Scene* parent, class Window* window);

	~QuadRenderer();

	void BindCamera(Camera* camera);

	void AddShader(Shader& shader);
	uint64_t AddTexture(const char* filePath);
	uint64_t AddTexture(unsigned char* data, int x, int y);

	std::pair<int, int> GetTextureDimension(uint64_t index) { return m_textureDimensions[index]; }
	int GetTextureWidth(uint64_t index) { return GetTextureDimension(index).first; }
	int GetTextureHeight(uint64_t index) { return GetTextureDimension(index).second; }

	void ClearResources();

	void Render(float ratio, int selectedIndex = -1);

	void ShowStatsWindow();

	std::unordered_map<uint64_t, Shader>&  GetShaders() { return m_shaders; }
	int GetTextureCount() { return m_textureCount; }
	std::vector<std::pair<int, int>>& GetTextureDimensions() { return m_textureDimensions; }
private:
	bool InWindow(class Quad& quad, float ratio);
	void RenderScreen();
	void Resize();
private:
	unsigned int m_vao, m_vbo, m_instanceBuffer, m_ebo, m_fbo, m_rbo, m_colBuf, m_textureArray;

	Shader m_screenShader;
	Shader m_shader;
	RenderQuad m_screen;

	unsigned int m_quadRendered = 0;
	Camera* m_camera = nullptr;
	Scene* m_parent = nullptr;
	Window* m_window = nullptr;
	int m_wx = 0, m_wy = 0;
	std::unordered_map<uint64_t, Shader> m_shaders{};
	// width, height
	std::vector<std::pair<int, int>> m_textureDimensions;
	int m_textureCount;

	std::vector<int> m_dstIndex;
	bool m_useTexture = true;

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