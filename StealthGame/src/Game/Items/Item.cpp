#include "Item.h"

void Item::Init(QuadRenderer* map, std::vector<QuadInitDesc>& descs)
{
	m_map = map;
	m_indices.reserve(descs.size());
	for (QuadInitDesc& desc : descs)
	{
		m_map->AddQuad(desc.m_pos, desc.m_scale, desc.m_shaderIndex, desc.m_textuerIndex);
		m_indices.emplace_back((int)m_map->GetQuads().size() - 1);
		m_map->GetQuads().back().GetAABB().SetEnabled(false);
	}
}