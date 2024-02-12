#include "QuadRenderer.h"

#include "../Scene.h"

QuadRenderer::~QuadRenderer()
{
	for (auto& [key, val] : m_parent->GetRenderQuads())
		val.Cleanup();

	for (auto& [key, val] : m_shaders)
		val.Cleanup();

	for (auto& [key, val] : m_textures)
		val.Cleanup();
}

void QuadRenderer::BindCamera(Camera* camera)
{
	m_camera = camera;
}

void QuadRenderer::AddShader(Shader& shader)
{
	m_shaders[shader.GetUUID().GetUUID()] = std::move(shader);
}

void QuadRenderer::AddTexture(Texture& texture)
{
	m_textures[texture.GetUUID().GetUUID()] = std::move(texture);
}

void QuadRenderer::DeleteTexture(uint64_t key)
{
	m_textures.erase(key);
}

void QuadRenderer::Render(float ratio, int selectedIndex)
{
	m_quadRendered = 0;

	RenderDesc desc;
	desc.m_camera = m_camera;
	desc.m_ratio = ratio;

	for (uint64_t& uuid : m_parent->GetDepthOrder())
	{
		RenderQuad& renderQuad = m_parent->GetRenderQuads()[uuid];
		renderQuad.UpdateRenderQuad(m_parent);

		Quad& quad = m_parent->GetQuads()[uuid];
		if (InWindow(quad, ratio) || renderQuad.GetFollowCamera() == false)
		{
			desc.m_shader = &m_shaders[renderQuad.getShaderUUID()];
			desc.m_texture = &m_textures[renderQuad.GetTextureUUID()];
			desc.m_isSelected = false;

			renderQuad.Draw(desc);
			if (renderQuad.GetVisible())
				m_quadRendered++;
		}
	}
}

void QuadRenderer::ShowStatsWindow()
{
#ifndef IMGUI_DISABLE
	ImGui::Begin("Renderer");
	ImGui::Text("Quad count: %u", m_parent->GetQuads().size());
	ImGui::Text("Quad Rendered: %u", m_quadRendered);
	ImGui::Text("Shaders Allocated: %u", m_shaders.size());
	ImGui::Text("Textures Allocated: %u", m_textures.size());
	ImGui::End();
#endif
}

bool QuadRenderer::InWindow(Quad& quad, float ratio)
{
	float right = quad.GetPos().x - m_camera->GetPosX() + quad.GetRadius().x;
	float left = quad.GetPos().x - m_camera->GetPosX() - quad.GetRadius().x;
	float top = quad.GetPos().y - m_camera->GetPosY() + quad.GetRadius().y;
	float bottom = quad.GetPos().y - m_camera->GetPosY() - quad.GetRadius().y;

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