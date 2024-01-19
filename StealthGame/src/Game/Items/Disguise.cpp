#include "Disguise.h"

void Disguise::Init(Type type, glm::vec2 pos, float depth, int shaderIndex)
{
	Scene* scene = GlobalData::Get().m_scene;

	m_type = type;

	Quad quad(pos, glm::vec2(0.2f, 0.2f));
	uint64_t uuid = quad.GetUUID().GetUUID();

	RenderQuadInitDesc renderDesc;
	renderDesc.m_depth = depth;
	renderDesc.m_shaderIndex = shaderIndex;
	renderDesc.m_textureIndex = 0;

	scene->AddQuad(quad, renderDesc);
	scene->GetRenderQuads()[uuid].UpdateRenderQuad(scene, uuid);
	scene->GetAABBs()[uuid].SetEnabled(false);

	m_uuid = uuid;
}

void Disguise::OnCollect(GameScene* scene)
{
	scene->GetItems().DeleteItem(m_uuid.GetUUID());
}