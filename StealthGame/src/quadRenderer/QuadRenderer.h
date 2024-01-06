#pragma once

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include "Quad.h"
#include "Shader.h"
#include "Texture.h"

#include "Camera.h"

class QuadRenderer
{
public:
	~QuadRenderer();

	void BindCamera(Camera* camera);

	void AddQuad(glm::vec2 pos, glm::vec2 scale, unsigned int shaderIndex, unsigned int textureIndex);
	void AddShader(const char* vertex, const char* fragment);
	void AddTexture(const char* texturePath);

	void Render(float ratio);

	void ShowStatsWindow();

	std::vector<Quad>& GetQuads() { return m_quads; }
private:
	bool InWindow(Quad& quad, float ratio);
private:
	unsigned int m_quadRendered = 0;
	Camera* m_camera = nullptr;
	std::vector<Quad> m_quads;
	std::vector<Shader> m_shaders;
	std::vector<Texture> m_textures;
};