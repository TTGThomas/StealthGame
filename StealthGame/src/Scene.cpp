#include "Scene.h"

Scene::~Scene()
{
}

void Scene::AddQuad(Quad& quad, RenderQuadInitDesc& renderQuadDesc)
{
	m_renderQuads[quad.GetUUID().GetUUID()] = RenderQuad(renderQuadDesc);
	m_aabb[quad.GetUUID().GetUUID()] = AABB(quad.GetPos() - quad.GetRadius(), quad.GetPos() + quad.GetRadius());
	m_quads[quad.GetUUID().GetUUID()] = std::move(quad);
}