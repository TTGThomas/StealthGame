#include "QuadRenderer.h"

void QuadRenderer::BindCamera(Camera* camera)
{
	m_camera = camera;
}

void QuadRenderer::AddQuad(glm::vec2 pos, glm::vec2 scale, unsigned int shaderIndex, unsigned int textureIndex)
{
	m_quads.emplace_back(pos, scale, shaderIndex, textureIndex);
}

void QuadRenderer::AddShader(const char* vertex, const char* fragment)
{
	m_shaders.emplace_back(vertex, fragment);
}

void QuadRenderer::AddTexture(const char* texturePath)
{
	m_textures.emplace_back(texturePath);
}

void QuadRenderer::Render(float ratio)
{
	for (Quad& quad : m_quads)
		quad.Draw(m_camera, &m_shaders[quad.getShaderIndex()], &m_textures[quad.GetTextureIndex()], ratio);
}

void QuadRenderer::ShowStatsWindow()
{
	ImGui::Begin("Renderer");
	ImGui::Text("Quad count: %u", m_quads.size());
	Quad& quad = m_quads[0];
	ImGui::DragFloat2("Position", glm::value_ptr(quad.GetPos()), 0.001f);
	ImGui::DragFloat2("Scale", glm::value_ptr(quad.GetScale()), 0.001f);
	ImGui::End();
}
