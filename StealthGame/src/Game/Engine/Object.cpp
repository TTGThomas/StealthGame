#include "Object.h"

Object::Object(std::vector<QuadInitDesc>& descs, bool isSpecial, bool isDoor)
{
	Init(descs, isSpecial, isDoor);
}

void Object::Init(std::vector<QuadInitDesc>& descs, bool isSpecial, bool isDoor)
{
	Scene* scene = GlobalData::Get().m_scene;

	m_uuids.reserve(descs.size());
	for (QuadInitDesc& desc : descs)
	{
		Quad quad(desc.m_pos, desc.m_scale);
		uint64_t uuid = quad.GetUUID().GetUUID();

		RenderQuadInitDesc renderDesc;
		renderDesc.m_depth = desc.m_depth;
		//renderDesc.m_depth = (float)rand() / (float)RAND_MAX;
		renderDesc.m_shaderUUID = desc.m_shaderUUID;
		renderDesc.m_textureUUID = desc.m_textureUUID;

		scene->AddQuad(quad, renderDesc);
		scene->GetRenderQuads()[uuid].UpdateRenderQuad(scene, uuid);
		scene->GetAABBs()[uuid].SetEnabled(false);

		m_uuids.emplace_back(uuid);
	}
	scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(true);

	GlobalData::Get().m_collision->AddToLayer(0, GetUUID(0).GetUUID());
	if (!isSpecial)
	{
		GlobalData::Get().m_collision->AddToLayer(3, GetUUID(0).GetUUID());
		GlobalData::Get().m_collision->AddToLayer(5, GetUUID(0).GetUUID());
	}
	else if (!isDoor)
		GlobalData::Get().m_collision->AddToLayer(5, GetUUID(0).GetUUID());
}