#include "QuadRenderer.h"

#include "../Scene.h"
#include "../windows/Window.h"

QuadRenderer::QuadRenderer(Scene* parent, Window* window)
	: m_parent(parent), m_window(window)
{
	m_wx = m_window->GetWidth();
	m_wy = m_window->GetHeight();

	// init frame buffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// Attach buffers

	// Create texture
	glGenTextures(1, &m_colBuf);
	glBindTexture(GL_TEXTURE_2D, m_colBuf);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_wx, m_wy, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	// bind
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colBuf, 0);

	// Create RBO(depth)
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, m_wx, m_wy);
	//GL_DEPTH24_STENCIL8;
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//bind
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
	//GL_DEPTH_STENCIL_ATTACHMENT;
	
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE)
		throw WildException("Frame buffer init failed");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Init screen
	m_screen.Init(0.5f, 0, 0);

	m_screenShader.Init("shaders/screen/Vertex.glsl", "shaders/screen/Fragment.glsl");
}

QuadRenderer::~QuadRenderer()
{
	ClearResources();

	m_screen.Cleanup();
	m_screenShader.Cleanup();
	glDeleteTextures(1, &m_colBuf);

	// delete frame buffer
	glDeleteFramebuffers(1, &m_fbo);
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
	m_textures[key].Cleanup();
	m_textures.erase(key);
}

void QuadRenderer::ClearResources()
{
	for (auto& [key, val] : m_parent->GetRenderQuads())
		val.Cleanup();

	for (auto& [key, val] : m_shaders)
		val.Cleanup();

	for (auto& [key, val] : m_textures)
		val.Cleanup();

	m_shaders.clear();
	m_textures.clear();
}

void QuadRenderer::Render(float ratio, int selectedIndex)
{
	Resize();

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
			desc.m_useTexture = m_useTexture;

			renderQuad.Draw(desc);
			if (renderQuad.GetVisible())
				m_quadRendered++;
		}
	}

	RenderScreen();
}

void QuadRenderer::ShowStatsWindow()
{
#ifndef IMGUI_DISABLE
	ImGui::Begin("Renderer");
	ImGui::Text("Quad count: %u", m_parent->GetQuads().size());
	ImGui::Text("Quad Rendered: %u", m_quadRendered);
	ImGui::Text("Shaders Allocated: %u", m_shaders.size());
	ImGui::Text("Textures Allocated: %u", m_textures.size());
	ImGui::Checkbox("Use Texture", &m_useTexture);
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

void QuadRenderer::RenderScreen()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);

	m_screenShader.Bind();
	glBindTexture(GL_TEXTURE_2D, m_colBuf);

	RenderDesc desc;
	desc.m_camera = nullptr;
	desc.m_isSelected = false;
	desc.m_ratio = 1.0f;
	desc.m_shader = nullptr;
	desc.m_texture = nullptr;
	desc.m_useTexture = true;
	m_screen.Draw(desc);

	m_screenShader.Unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void QuadRenderer::Resize()
{
	if (m_wx != m_window->GetWidth() || m_wy != m_window->GetHeight())
	{
		m_wx = m_window->GetWidth();
		m_wy = m_window->GetHeight();

		// delete
		glDeleteTextures(1, &m_colBuf);
		glDeleteRenderbuffers(1, &m_rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		// create new texture and depth buffer
		glGenTextures(1, &m_colBuf);
		glBindTexture(GL_TEXTURE_2D, m_colBuf);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_wx, m_wy, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		// bind
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colBuf, 0);

		// Create RBO(depth)
		glGenRenderbuffers(1, &m_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, m_wx, m_wy);
		//GL_DEPTH24_STENCIL8;

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		//bind
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
		//GL_DEPTH_STENCIL_ATTACHMENT;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}