#include "Fist.h"

uint64_t Fist::m_texHUD;
uint64_t Fist::m_texInGame;

// a lot of things are hard coded

void Fist::OnEquip()
{
	GlobalData gData = GlobalData::Get();

	if (m_texHUD == 0)
		m_texHUD = CreateTexture("res/Inventories/fistHUD.png");
	if (m_texInGame == 0)
		m_texInGame = CreateTexture("res/Inventories/fist.png");
	AddQuad({}, {0.2f, 0.2f}, false, m_texHUD);
	AddQuad({}, {0.2f, 0.2f}, true, m_texInGame);
}

void Fist::OnUnequip()
{
	GlobalData gData = GlobalData::Get();

	gData.m_scene->DeleteQuad(m_uuids[0].GetUUID());
	gData.m_scene->DeleteQuad(m_uuids[1].GetUUID());
	m_uuids = {};
}

void Fist::OnEquipping()
{
	GlobalData gData = GlobalData::Get();

	gData.m_scene->GetQuads()[m_uuids[0].GetUUID()].SetPos(m_HUDpos);
}

void Fist::ClearResources()
{
	m_texHUD = 0;
	m_texInGame = 0;
}

void Fist::OnResize(int x, int y)
{
	GlobalData gData = GlobalData::Get();

	float ratio = (float)y / (float)x;
	m_HUDpos = { (1.0f / ratio) - 0.2f, -0.8f };
}

bool Fist::IsIllegal()
{
	return false;
}
