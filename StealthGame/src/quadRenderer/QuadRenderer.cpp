#include "QuadRenderer.h"

QuadRenderer::~QuadRenderer()
{
	for (Quad& quad : m_quads)
		quad.Cleanup();
}

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
	m_quadRendered = 0;
	for (Quad& quad : m_quads)
	{
		if (InWindow(quad, ratio))
		{
			quad.Draw(m_camera, &m_shaders[quad.getShaderIndex()], &m_textures[quad.GetTextureIndex()], ratio);
			m_quadRendered++;
		}
	}
}

void QuadRenderer::ShowStatsWindow()
{
	ImGui::Begin("Renderer");
	ImGui::Text("Quad count: %u", m_quads.size());
	ImGui::Text("Quad Rendered: %u", m_quadRendered);
	Quad& quad = m_quads.back();
	ImGui::DragFloat2("Position", glm::value_ptr(quad.GetPos()), 0.001f);
	ImGui::DragFloat2("Scale", glm::value_ptr(quad.GetScale()), 0.001f);
	ImGui::End();
}

bool QuadRenderer::InWindow(Quad& quad, float ratio)
{
	float right = quad.GetPos().x - m_camera->GetPosX() + quad.GetScale().x;
	float left = quad.GetPos().x - m_camera->GetPosX() - quad.GetScale().x;
	float top = quad.GetPos().y - m_camera->GetPosY() + quad.GetScale().y;
	float bottom = quad.GetPos().y - m_camera->GetPosY() - quad.GetScale().y;

	right *= m_camera->GetZoom();
	left *= m_camera->GetZoom();
	top *= m_camera->GetZoom();
	bottom *= m_camera->GetZoom();

	if (right * ratio < -1.0f)
		return false;
	else if (left * ratio > 1.0f)
		return false;
	else if (top < -1.0f)
		return false;
	else if (bottom > 1.0f)
		return false;

	return true;
}
