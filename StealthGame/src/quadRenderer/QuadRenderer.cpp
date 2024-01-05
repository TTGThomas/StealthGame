#include "QuadRenderer.h"

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

void QuadRenderer::Render()
{
	for (Quad& quad : m_quads)
		quad.Draw(&m_shaders[quad.getShaderIndex()], &m_textures[quad.GetTextureIndex()]);
}