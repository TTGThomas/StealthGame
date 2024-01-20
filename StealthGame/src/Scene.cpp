#include "Scene.h"

Scene::~Scene()
{
}

void Scene::AddQuad(Quad& quad, RenderQuadInitDesc& renderQuadDesc)
{
	m_renderQuads[quad.GetUUID().GetUUID()] = std::move(RenderQuad(renderQuadDesc));
	m_aabb[quad.GetUUID().GetUUID()] = AABB(quad.GetPos() - quad.GetRadius(), quad.GetPos() + quad.GetRadius());
	m_quads[quad.GetUUID().GetUUID()] = std::move(quad);

	// depth order is from the largest to the smallest
	int index = 0;
	for (int i = 0; i < m_depthOrder.size(); i++)
	{
		if (renderQuadDesc.m_depth > m_renderQuads[m_depthOrder[i]].GetDepth())
		{
			index = i;
			break;
		}
	}

	m_depthOrder.emplace_back(quad.GetUUID().GetUUID());
	m_depthOrder.insert(m_depthOrder.begin() + index, quad.GetUUID().GetUUID());
}