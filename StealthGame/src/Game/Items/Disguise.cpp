#include "Disguise.h"

void Disguise::Init(Identities type, glm::vec2 pos, float depth, uint64_t shaderUUID)
{
	Scene* scene = GlobalData::Get().m_scene;

	m_type = type;

	Quad quad;
	uint64_t uuid = quad.GetUUID().GetUUID();
	quad.SetPos(pos);
	quad.SetRadius({ 0.2f, 0.2f });

	RenderQuadInitDesc renderDesc;
	renderDesc.m_depth = depth;
	renderDesc.m_shaderUUID = shaderUUID;
	renderDesc.m_textureUUID = DisguiseFromIdentity(type);

	scene->AddQuad(quad, renderDesc);
	scene->GetRenderQuads()[uuid].UpdateRenderQuad(scene, uuid);
	scene->GetRenderQuads()[uuid].SetSideFrames(4);
	scene->GetRenderQuads()[uuid].SetFrameIndex(0);
	scene->GetAABBs()[uuid].SetEnabled(false);

	m_uuid = uuid;
}

void Disguise::OnCollect(GameScene* scene)
{
	GlobalData& gData = GlobalData::Get();
	Player& player = gData.m_gameScene->GetPlayer();

	Identities type = m_type;
	m_type = player.GetDisguise();
	player.ChangeDisguise(type);

	gData.m_scene->GetRenderQuads()[GetUUID().GetUUID()].SetTextureUUID(
		DisguiseFromIdentity(m_type)
	);
	gData.m_scene->GetQuads()[GetUUID().GetUUID()].SetPos(player.GetPos());
}

uint64_t Disguise::DisguiseFromIdentity(Identities type)
{
	GlobalData& gData = GlobalData::Get();

	if (type == Identities::STANDARD)
		return gData.m_texPlayer;
	else if (type == Identities::GUEST)
		return gData.m_texNPC0;
	else if (type == Identities::VIPGUEST)
		return gData.m_texNPC1;
	else if (type == Identities::GUARD)
		return gData.m_texNPC2;
	else if (type == Identities::VIPGUARD)
		return gData.m_texNPC3;
	return 0;
}

uint64_t Disguise::TexFromIdentity(Identities type)
{
	return 0;
}
