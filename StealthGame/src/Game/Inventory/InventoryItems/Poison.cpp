#include "Poison.h"

#include "../../GameScene.h"

uint64_t Poison::m_texHUD;
uint64_t Poison::m_texInGame;

void Poison::OnEquip()
{
	GlobalData gData = GlobalData::Get();

	if (m_texHUD == 0)
		m_texHUD = CreateTexture("res/Inventories/poisonHUD.png");
	if (m_texInGame == 0)
		m_texInGame = CreateTexture("res/Inventories/poison.png");
	AddQuad({}, { MAP_RADIUS, MAP_RADIUS }, false, m_texHUD);
	AddQuad({}, { MAP_RADIUS, MAP_RADIUS }, true, m_texInGame);
}

void Poison::OnUnequip()
{
	GlobalData gData = GlobalData::Get();

	gData.m_scene->DeleteQuad(m_uuids[0].GetUUID());
	gData.m_scene->DeleteQuad(m_uuids[1].GetUUID());
	m_uuids = {};
}

void Poison::OnEquipping()
{
	GlobalData gData = GlobalData::Get();
	Player& player = gData.m_gameScene->GetPlayer();

	gData.m_scene->GetQuads()[m_uuids[0].GetUUID()].SetPos(m_HUDpos);

	{
		glm::vec2 pos = gData.m_gameScene->GetPlayer().GetPos();
		gData.m_scene->GetQuads()[m_uuids[1].GetUUID()].SetPos(pos);
	}
}

void Poison::OnResize(int x, int y)
{
	GlobalData gData = GlobalData::Get();

	m_windowScale = { x, y };
	float ratio = (float)y / (float)x;
	m_HUDpos = { (1.0f / ratio) - 0.2f, -0.8f };
}

bool Poison::IsIllegal()
{
	return true;
}

void Poison::ClearResources()
{
	m_texHUD = 0;
	m_texInGame = 0;
}