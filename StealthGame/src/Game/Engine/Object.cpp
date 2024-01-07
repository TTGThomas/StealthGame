#include "Object.h"

Object::Object(QuadRenderer* map, std::vector<QuadInitDesc>& descs)
{
	Init(map, descs);
}

void Object::Init(QuadRenderer* map, std::vector<QuadInitDesc>& descs)
{
	m_map = map;
	m_indices.reserve(descs.size());
	for (QuadInitDesc& desc : descs)
	{
		m_map->AddQuad(desc.m_pos, desc.m_scale, desc.m_shaderIndex, desc.m_textuerIndex);
		m_indices.emplace_back((int)m_map->GetQuads().size() - 1);
		m_map->GetQuads().back().GetAABB().SetEnabled(false);
	}
	GetQuad(0)->GetAABB().SetEnabled(true);
	GetQuad(0)->SetVisibility(false);
}