#include "Quad.h"

Quad::Quad(glm::vec2 pos, glm::vec2 scale, unsigned int shaderIndex, unsigned int textureIndex)
{
	Init(pos, scale, shaderIndex, textureIndex);
}

Quad::~Quad()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
}

void Quad::Init(glm::vec2 pos, glm::vec2 scale, unsigned int shaderIndex, unsigned int textureIndex)
{
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

void Quad::Draw(Shader* shader, Texture* texture)
{
	shader->Bind();
	texture->Bind();

	glUniform2f(LOCATION(*shader, "u_pos"), m_pos.x, m_pos.y);
	glUniform2f(LOCATION(*shader, "u_scale"), m_scale.x, m_scale.y);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	texture->Unbind();
	shader->Unbind();
}
