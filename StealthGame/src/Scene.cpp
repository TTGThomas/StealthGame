#include "Scene.h"

#include "quadRenderer/Texture.h"

Scene::~Scene()
{
}

void Scene::AddQuad(Quad& quad, RenderQuadInitDesc& renderQuadDesc)
{
	m_renderQuads[quad.GetUUID().GetUUID()] = std::move(RenderQuad(renderQuadDesc));
	m_aabb[quad.GetUUID().GetUUID()] = AABB(quad.GetPos() - quad.GetRadius(), quad.GetPos() + quad.GetRadius());
	m_quads[quad.GetUUID().GetUUID()] = std::move(quad);

	int index = 0;
	for (int i = 0; i < m_depthOrder.size(); i++)
	{
		if (renderQuadDesc.m_depth > m_renderQuads[m_depthOrder[i]].GetDepth())
		{
			index = i;
			break;
		}
	}

	m_depthOrder.insert(m_depthOrder.begin() + index, quad.GetUUID().GetUUID());
}

void Scene::DeleteQuad(uint64_t uuid)
{
	m_quads.erase(uuid);
	m_renderQuads.erase(uuid);
	m_aabb.erase(uuid);

	for (int i = 0; i < m_depthOrder.size(); i++)
	{
		if (m_depthOrder[i] == uuid)
		{
			m_depthOrder.erase(m_depthOrder.begin() + i);
			break;
		}
	}
}

uint64_t Scene::GetTextureFromChar(QuadRenderer* renderer, char text)
{
	int aWidth, aHeight, nChannels;
	unsigned char* data = Texture::GetDataFromFile("res/Fonts.sample-atlas.png", &aWidth, &aHeight, &nChannels);
	const int width = 24, height = 24;

	unsigned char* ret = new unsigned char[24 * 24 * sizeof(unsigned char)];
	
	int index = 0;
	int texIndex = (int)(text - ' ');

	int sourceX = (texIndex % 10) * 25;
	int sourceY = (texIndex / 10) * 25;
	for (int y = 0; y < 24; y++)
	{
		for (int x = 0; x < 24; x++)
		{
			int posX = sourceX + x;
			int posY = sourceY + y;
			ret[index] = data[posY * aHeight + posX];
			++index;
		}
	}

	Texture texture;
	texture.Init(ret, width, height);
	renderer->AddTexture(texture);
	delete[] data, ret;
	return texture.GetUUID().GetUUID();
}