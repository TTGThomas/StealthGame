#include "Quad.h"

Quad::Quad(glm::vec2 pos, glm::vec2 scale, unsigned int shaderIndex, unsigned int textureIndex)
{
	Init(pos, scale, shaderIndex, textureIndex);
}

void Quad::Cleanup()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
}

void Quad::Init(glm::vec2 pos, glm::vec2 scale, unsigned int shaderIndex, unsigned int textureIndex)
{
	m_aabb = AABB(pos - scale, pos + scale);

	m_pos = pos;
	m_scale = scale;
	m_shaderIndex = shaderIndex;
	m_textureIndex = textureIndex;

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_pos));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_texCoord));

	glBindVertexArray(0);
}

void Quad::Draw(RenderDesc& desc)
{
	if (!m_visible)
		return;

	desc.m_shader->Bind();
	desc.m_texture->Bind();

	glUniform1f(LOCATION(*desc.m_shader, "u_screenRatio"), desc.m_ratio);

	glUniform2f(LOCATION(*desc.m_shader, "u_pos"), m_pos.x - desc.m_camera->GetPosX(), m_pos.y - desc.m_camera->GetPosY());
	glUniform2f(LOCATION(*desc.m_shader, "u_scale"), m_scale.x, m_scale.y);

	glUniform1f(LOCATION(*desc.m_shader, "u_cameraZoom"), desc.m_camera->GetZoom());
	glUniform1i(LOCATION(*desc.m_shader, "u_selected"), desc.m_isSelected);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	desc.m_texture->Unbind();
	desc.m_shader->Unbind();
}

void Quad::UpdateAABB()
{
	m_aabb.SetMinPos(m_pos - m_scale);
	m_aabb.SetMaxPos(m_pos + m_scale);
}
