#include "RenderQuad.h"

#include "../Scene.h"

RenderQuad::RenderQuad(RenderQuadInitDesc& desc, GameUUID uuid)
{
	m_uuid = uuid;
	m_followCamera = desc.m_followCameraOffset;
	Init(desc.m_depth, desc.m_shaderUUID, desc.m_textureUUID);
}

void RenderQuad::Cleanup()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
}

void RenderQuad::Init(float depth, uint64_t shaderUUID, uint64_t textureUUID)
{
	m_shaderUUID = shaderUUID;
	m_textureUUID = textureUUID;
	m_depth = depth;

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

	glBindVertexArray(0);
}

void RenderQuad::Draw(RenderDesc& desc)
{
	if (m_screenRatioLoc == -1)
	{
		m_screenRatioLoc = LOCATION(*desc.m_shader, "u_screenRatio");
		m_matrixLoc = LOCATION(*desc.m_shader, "u_matrix");
		m_alphaLoc = LOCATION(*desc.m_shader, "u_alpha");
		m_depthLoc = LOCATION(*desc.m_shader, "u_depth");
		m_frameIndexLoc = LOCATION(*desc.m_shader, "u_frameIndex");
		m_sideFramesLoc = LOCATION(*desc.m_shader, "u_sideFrames");
		m_useTextureLoc = LOCATION(*desc.m_shader, "u_useTexture");
	}

	if (!m_visible)
		return;

	if (m_alpha == 0.0f)
		return;

	desc.m_shader->Bind();
	desc.m_texture->Bind();

	glUniform1f(m_screenRatioLoc, desc.m_ratio);

	glm::mat4 matrix = glm::identity<glm::mat4>();

	// view
	if (m_followCamera)
	{
		matrix = glm::scale(matrix, glm::vec3(glm::vec2(desc.m_camera->GetZoom()), 1.0f));
		matrix = glm::translate(matrix, { -desc.m_camera->GetPosX(), -desc.m_camera->GetPosY(), 0.0f });
	}

	// model
	matrix = glm::translate(matrix, { m_pos.x, m_pos.y, 0.0f });
	matrix = glm::rotate(matrix, glm::radians(-m_rotation), {0.0f, 0.0f, 1.0f});
	matrix = glm::scale(matrix, { m_radius.x, m_radius.y, 1.0f });

	glUniformMatrix4fv(m_matrixLoc, 1, GL_FALSE, glm::value_ptr(matrix));
	
	glUniform1f(m_alphaLoc, m_alpha);
	glUniform1f(m_depthLoc, m_depth);

	glUniform1i(m_frameIndexLoc, (int)m_frameIndex);
	glUniform1i(m_sideFramesLoc, m_sideFrames);

	glUniform1i(m_useTextureLoc, desc.m_useTexture ? 1: 0);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	desc.m_texture->Unbind();
	desc.m_shader->Unbind();
}

void RenderQuad::UpdateRenderQuad(Scene* scene)
{
	Quad& quad = scene->GetQuads()[m_uuid.GetUUID()];
	m_pos = quad.GetPos();
	m_radius = quad.GetRadius();
	m_rotation = quad.GetRotation();
}

void RenderQuad::UpdateRenderQuad(Scene* scene, GameUUID uuid)
{
	m_uuid = uuid;

	Quad& quad = scene->GetQuads()[m_uuid.GetUUID()];
	m_pos = quad.GetPos();
	m_radius = quad.GetRadius();
	m_rotation = quad.GetRotation();
}