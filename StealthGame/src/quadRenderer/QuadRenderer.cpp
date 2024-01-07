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

void QuadRenderer::Render(float ratio, int selectedIndex)
{
	m_quadRendered = 0;

	RenderDesc desc;
	desc.m_camera = m_camera;
	desc.m_ratio = ratio;

	for (int i = 0; i < m_quads.size(); i++)
	{
		Quad& quad = m_quads[i];
		if (InWindow(quad, ratio))
		{
			desc.m_shader = &m_shaders[quad.getShaderIndex()];
			desc.m_texture = &m_textures[quad.GetTextureIndex()];
			desc.m_isSelected = (i == selectedIndex);

			quad.Draw(desc);
			m_quadRendered++;
		}
	}
}

void QuadRenderer::ShowStatsWindow(int controlIndex)
{
#ifndef IMGUI_DISABLE
	ImGui::Begin("Renderer");
	ImGui::Text("Quad count: %u", m_quads.size());
	ImGui::Text("Quad Rendered: %u", m_quadRendered);
	if (controlIndex != -1.0f)
	{
		Quad& quad = m_quads[controlIndex];
		ImGui::DragFloat2("Position", glm::value_ptr(quad.GetPos()), 0.001f);
		ImGui::DragFloat2("Scale", glm::value_ptr(quad.GetScale()), 0.001f);
	}
	ImGui::End();
#endif
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
