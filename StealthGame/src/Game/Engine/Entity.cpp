#include "Entity.h"

void Entity::Init(std::vector<QuadInitDesc>& descs, bool isSpecial, bool isDoor)
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
	scene->GetRenderQuads()[GetUUID(0).GetUUID()].SetVisibility(false);

	GlobalData::Get().m_collision->AddToLayer(1, GetUUID(0).GetUUID());
}

void Entity::Move(CollisionDetector* collision, int layer, float x, float y)
{
	GetQuad(0)->ChangePos({ x, 0 });
	if (collision->Collide(layer, GetUUID(0)).m_hasHit)
		GetQuad(0)->ChangePos({ -x, 0 });

	GetQuad(0)->ChangePos({ 0, y });
	if (collision->Collide(layer, GetUUID(0)).m_hasHit)
		GetQuad(0)->ChangePos({ 0, -y });
}