#include "Object.h"

Object::Object(std::vector<QuadInitDesc>& descs)
{
	Init(descs);
}

void Object::Init(std::vector<QuadInitDesc>& descs)
{
	Scene* scene = GlobalData::Get().m_scene;

	m_uuids.reserve(descs.size());
	for (QuadInitDesc& desc : descs)
	{
		Quad quad(desc.m_pos, desc.m_scale);
		uint64_t uuid = quad.GetUUID().GetUUID();

		RenderQuadInitDesc renderDesc;
		renderDesc.m_depth = desc.m_depth;
		renderDesc.m_shaderUUID = desc.m_shaderUUID;
		renderDesc.m_textureUUID = desc.m_textureUUID;

		scene->AddQuad(quad, renderDesc);
		scene->GetRenderQuads()[uuid].UpdateRenderQuad(scene, uuid);
		scene->GetAABBs()[uuid].SetEnabled(false);

		m_uuids.emplace_back(uuid);
	}
	scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(true);
	scene->GetRenderQuads()[GetUUID(0).GetUUID()].SetVisibility(false);
}