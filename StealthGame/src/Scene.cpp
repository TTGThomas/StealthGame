#include "Scene.h"

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