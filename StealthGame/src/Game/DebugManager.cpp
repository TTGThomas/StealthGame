#include "DebugManager.h"

std::vector<DebugManager::DebugObjectDesc> DebugManager::m_objects;

void DebugManager::AddDebugQuad(glm::vec2 pos, glm::vec2 radius, float rotation)
{
	m_objects.emplace_back(DebugObjectDesc(pos, radius, rotation));
}

void DebugManager::RenderDebugs()
{
	for (DebugObjectDesc& object : m_objects)
	{
		GlobalData& gData = GlobalData::Get();
		Quad quad;
		uint64_t uuid = quad.GetUUID().GetUUID();
		quad.SetPos(object.m_pos);
		quad.SetRadius(object.m_radius);
		quad.SetRotation(object.m_rotation);
		RenderQuadInitDesc desc;
		desc.m_depth = 0.7f;
		desc.m_followCameraOffset = true;
		desc.m_shaderUUID = gData.m_defaultShader;
		desc.m_textureUUID = gData.m_texLogo;
		gData.m_scene->AddQuad(quad, desc);
		gData.m_scene->GetAABBs()[uuid].SetEnabled(false);
	}
	m_objects.clear();
}
