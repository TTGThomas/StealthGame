#include "Scene.h"

#include "quadRenderer/Texture.h"

int Scene::m_letterWidths[100] = {
		 6, 3, 5,10, 9,13,17, 3, 5, 4,
		 6, 9, 3,19, 3, 5, 8, 5, 8, 8,
		 8, 8, 8, 8, 8, 8, 3, 3, 9, 9,
		 9, 8,15,10,10,11,11,10, 9,12,
		11, 3, 8,10, 8,13,11,12,10,12,
		12,10, 9,10,10,13,11,11, 9, 5,
		 6, 5, 8,15, 4, 8, 8, 8, 8, 8,
		 6, 8, 7, 3, 4, 8, 3,13, 8, 8,
		 8, 8, 6, 8, 6, 8, 8,13, 9, 8,
		 8, 5, 3, 5, 9,13,10,13, 6, 7,
};

Scene::~Scene()
{
}

void Scene::AddQuad(Quad& quad, RenderQuadInitDesc& renderQuadDesc)
{
	uint64_t uuid = quad.GetUUID().GetUUID();
	m_renderQuads.insert({ uuid, RenderQuad(renderQuadDesc, uuid) });
	m_aabb.insert({ uuid, AABB(quad.GetPos() - quad.GetRadius(), quad.GetPos() + quad.GetRadius(), uuid) });
	m_quads.insert({ uuid, quad });

	int index = -1;
	for (int i = 0; i < m_depthOrder.size(); i++)
	{
		if (renderQuadDesc.m_depth < m_renderQuads[m_depthOrder[i]].GetDepth())
		{
			index = i;
			break;
		}
	}

	if (index != -1)
		m_depthOrder.insert(m_depthOrder.begin() + index, uuid);
	else
		m_depthOrder.emplace_back(uuid);
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

uint64_t Scene::GetTextureFromChar(QuadRenderer* renderer, glm::vec3 color, char text)
{
	int aWidth, aHeight, nChannels;
	unsigned char* data = Texture::GetDataFromFile("res/Fonts/sample-atlas.png", &aWidth, &aHeight, &nChannels);

	int texIndex = (int)(text - ' ');
	if (texIndex < 0)
		texIndex = 95;

	const int width = m_letterWidths[texIndex], height = 24;
	//const int width = 24, height = 24;
	unsigned char* ret = new unsigned char[width * height * sizeof(unsigned char) * 4];

	int index = 0;
	int sourceX = (texIndex % 10) * 25 + 1;
	int sourceY = (texIndex / 10) * 25 + 1;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int posX = sourceX + x;
			int posY = sourceY + y;
			ret[index + 0] = (unsigned char)((float)data[(posY * aHeight + posX) * 4 + 0] * color.r);
			ret[index + 1] = (unsigned char)((float)data[(posY * aHeight + posX) * 4 + 1] * color.g);
			ret[index + 2] = (unsigned char)((float)data[(posY * aHeight + posX) * 4 + 2] * color.b);
			ret[index + 3] = (unsigned char)((float)data[(posY * aHeight + posX) * 4 + 3] * 1.0f);
			index += 4;
		}
	}

	Texture texture;
	texture.Init(ret, width, height);
	renderer->AddTexture(texture);
	delete[] data, ret;
	return texture.GetUUID().GetUUID();
}