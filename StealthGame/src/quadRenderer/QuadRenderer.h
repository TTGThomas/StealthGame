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

class QuadRenderer
{
public:
	QuadRenderer(class Scene* parent)
		: m_parent(parent) {}

	~QuadRenderer();

	void BindCamera(Camera* camera);

	void AddShader(Shader& shader);
	void AddTexture(Texture& texture);

	void Render(float ratio, int selectedIndex = -1);

	void ShowStatsWindow();
private:
	bool InWindow(class Quad& quad, float ratio);
private:
	unsigned int m_quadRendered = 0;
	Camera* m_camera = nullptr;
	Scene* m_parent = nullptr;
	std::unordered_map<uint64_t, Shader> m_shaders{};
	std::unordered_map<uint64_t, Texture> m_textures{};

	std::vector<int> m_dstIndex;
};