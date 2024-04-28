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

unsigned char* Scene::m_letterAtlas = nullptr;
int Scene::m_atlasWidth = -1;
int Scene::m_atlasHeight = -1;
int Scene::m_atlasNChannels = -1;
std::array<uint64_t, 100> Scene::m_letterTextures; 

Scene::Scene()
{
	m_letterTextures.fill(0);
}

Scene::~Scene()
{
	m_audio.Cleanup();
	delete[] m_letterAtlas;
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

void Scene::UpdateDepthOrder()
{
	// bubble sort
	if (m_depthOrder.size() < 2)
		return;

	bool valChanged = false;
	do
	{
		int ptr1 = 0, ptr2 = 1;
		for (int ptr1 = 0, ptr2 = 1; ptr2 < m_depthOrder.size(); ptr2++, ptr1++)
		{
			float ptr1Depth = m_renderQuads[m_depthOrder[ptr1]].GetDepth();
			float ptr2Depth = m_renderQuads[m_depthOrder[ptr2]].GetDepth();

			if (ptr1Depth > ptr2Depth)
			{
				valChanged = true;

				uint64_t ptr1Val = m_depthOrder[ptr1];
				m_depthOrder[ptr1] = m_depthOrder[ptr2];
				m_depthOrder[ptr2] = ptr1Val;
			}
		}
	} while (valChanged);
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

void Scene::ClearResources()
{
	m_audio.Cleanup();
	m_quads.clear();
	m_renderQuads.clear();
	m_aabb.clear();
	m_depthOrder.clear();
	m_letterTextures = {};
}

uint64_t Scene::GetTextureFromChar(QuadRenderer* renderer, glm::vec3 color, char text)
{
	if (m_letterAtlas == nullptr)
		m_letterAtlas = Texture::GetDataFromFile("res/Fonts/sample-atlas.png", &m_atlasWidth, &m_atlasHeight, &m_atlasNChannels);

	int texIndex = (int)(text - ' ');
	if (texIndex < 0)
		texIndex = 95;

	if (m_letterTextures[texIndex] == 0)
	{
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
				ret[index + 0] = (unsigned char)((float)m_letterAtlas[(posY * m_atlasHeight + posX) * 4 + 0] * color.r);
				ret[index + 1] = (unsigned char)((float)m_letterAtlas[(posY * m_atlasHeight + posX) * 4 + 1] * color.g);
				ret[index + 2] = (unsigned char)((float)m_letterAtlas[(posY * m_atlasHeight + posX) * 4 + 2] * color.b);
				ret[index + 3] = (unsigned char)((float)m_letterAtlas[(posY * m_atlasHeight + posX) * 4 + 3] * 1.0f);
				index += 4;
			}
		}

		Texture texture;
		texture.Init(ret, width, height);
		renderer->AddTexture(texture);
		delete[] ret;
		m_letterTextures[texIndex] = texture.GetUUID().GetUUID();
	}
	return m_letterTextures[texIndex];
}