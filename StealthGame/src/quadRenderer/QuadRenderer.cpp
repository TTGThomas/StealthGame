#include "QuadRenderer.h"

#include "stb/stb_image.h"

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

	m_screenShader.Init("shaders/screen/Vertex.glsl", "shaders/screen/Fragment.glsl");
	m_shader.Init("shaders/Instance/Vertex.glsl", "shaders/Instance/Fragment.glsl");

	// create instance buffer
	glGenBuffers(1, &m_instanceBuffer);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * 10000, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Instancing
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_pos));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_texCoord));

	// bind instance buffer

	glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, m_matrix0));
	
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, m_matrix1));
	
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, m_matrix2));
	
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, m_matrix3));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, m_texDimension));

	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, m_texIndex));

	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, m_alpha));

	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, m_depth));

	glEnableVertexAttribArray(10);
	glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, m_sideFrames));

	glEnableVertexAttribArray(11);
	glVertexAttribPointer(11, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, m_frameIndex));
	
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glVertexAttribDivisor(9, 1);
	glVertexAttribDivisor(10, 1);
	glVertexAttribDivisor(11, 1);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// Create texture array
	glGenTextures(1, &m_textureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 256, 256, 256);
}

QuadRenderer::~QuadRenderer()
{
	ClearResources();

	m_screen.Cleanup();
	m_screenShader.Cleanup();
	m_shader.Cleanup();

	glDeleteTextures(1, &m_colBuf);
	glDeleteFramebuffers(1, &m_fbo);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
	glDeleteBuffers(1, &m_instanceBuffer);
}

void QuadRenderer::BindCamera(Camera* camera)
{
	m_camera = camera;
}

void QuadRenderer::AddShader(Shader& shader)
{
	m_shaders[shader.GetUUID().GetUUID()] = std::move(shader);
}

uint64_t QuadRenderer::AddTexture(const char* filePath)
{
	int x, y, n;
	unsigned char* data = stbi_load(filePath, &x, &y, &n, 4);
	uint64_t ret = AddTexture(data, x, y);
	stbi_image_free(data);
	return ret;
}

uint64_t QuadRenderer::AddTexture(unsigned char* data, int x, int y)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);
	m_textureDimensions.push_back({ x, y });
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_textureCount, x, y, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0); 
	return m_textureCount++;
}

void QuadRenderer::ClearResources()
{
	for (auto& [key, val] : m_parent->GetRenderQuads())
		val.Cleanup();

	for (auto& [key, val] : m_shaders)
		val.Cleanup();

	m_shaders.clear();

	glDeleteTextures(1, &m_textureArray);

	glGenTextures(1, &m_textureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 256, 256, 256);
}

void QuadRenderer::Render(float ratio, int selectedIndex)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	Resize();
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_quadRendered = 0;

	glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);
	int index = 0;
	for (uint64_t& uuid : m_parent->GetDepthOrder())
	{
		RenderQuad& renderQuad = m_parent->GetRenderQuads()[uuid];
		renderQuad.UpdateRenderQuad(m_parent);
	
		Quad& quad = m_parent->GetQuads()[uuid];
		if (InWindow(quad, ratio) || renderQuad.GetFollowCamera() == false)
		{
			if (renderQuad.GetVisible())
			{
				glm::mat4 matrix = renderQuad.Matrix(m_camera);
				InstanceData data;
				data.m_matrix0 = matrix[0];
				data.m_matrix1 = matrix[1];
				data.m_matrix2 = matrix[2];
				data.m_matrix3 = matrix[3];
				data.m_texDimension = { GetTextureWidth(renderQuad.GetTextureUUID()), GetTextureHeight(renderQuad.GetTextureUUID()) };
				data.m_texIndex = (float)renderQuad.GetTextureUUID();
				data.m_alpha = renderQuad.GetAlpha();
				data.m_depth = renderQuad.GetDepth();
				data.m_sideFrames = (int)renderQuad.GetSideFrames();
				data.m_frameIndex = (int)renderQuad.GetFrameIndex();
				glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(InstanceData), sizeof(InstanceData), &data);
				index++;
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);
	//int index = 0;
	//for (int y = -5; y < 5; y++)
	//{
	//	for (int x = -5; x < 5; x++)
	//	{
	//		glm::mat4 matrix = glm::identity<glm::mat4>();
	//		matrix = glm::translate(matrix, glm::vec3((float)x / 10.0f, (float)y / 10.0f, 0.0f));
	//		matrix = glm::scale(matrix, glm::vec3(0.06f, 0.06f, 1.0f));
	//
	//		glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(matrix));
	//		index++;
	//	}
	//}
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_shader.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);

	glUniform1f(LOCATION(m_shader, "u_screenRatio"), ratio);
	glUniform1i(LOCATION(m_shader, "u_useTexture"), m_useTexture);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, index);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	m_shader.Unbind();

	RenderScreen();
}

void QuadRenderer::ShowStatsWindow()
{
#ifndef IMGUI_DISABLE
	ImGui::Begin("Renderer");
	ImGui::Text("Quad count: %u", m_parent->GetQuads().size());
	ImGui::Text("Quad Rendered: %u", m_quadRendered);
	ImGui::Text("Shaders Allocated: %u", m_shaders.size());
	ImGui::Text("Textures Allocated: %u", m_textureCount);
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

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

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