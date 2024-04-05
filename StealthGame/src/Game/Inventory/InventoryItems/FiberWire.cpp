#include "FiberWire.h"

#include "../../GameScene.h"

uint64_t FiberWire::m_texHUD;
uint64_t FiberWire::m_texInGame;

void FiberWire::OnEquip()
{
	GlobalData gData = GlobalData::Get();

	if (m_texHUD == 0)
		m_texHUD = CreateTexture("res/Inventories/fiberWireHUD.png");
	if (m_texInGame == 0)
		m_texInGame = CreateTexture("res/Inventories/fiberWire.png");
	AddQuad({}, { 0.2f, 0.2f }, false, m_texHUD);
	AddQuad({}, { 0.2f, 0.2f }, true, m_texInGame);
}

void FiberWire::OnUnequip()
{
	GlobalData gData = GlobalData::Get();

	gData.m_scene->DeleteQuad(m_uuids[0].GetUUID());
	gData.m_scene->DeleteQuad(m_uuids[1].GetUUID());
	m_uuids = {};
}

void FiberWire::OnEquipping()
{
	GlobalData gData = GlobalData::Get();

	gData.m_scene->GetQuads()[m_uuids[0].GetUUID()].SetPos(m_HUDpos);

	{
		glm::vec2 pos = gData.m_gameScene->GetPlayer().GetPos();
		gData.m_scene->GetQuads()[m_uuids[1].GetUUID()].SetPos(pos);
	}
}

void FiberWire::OnResize(int x, int y)
{
	GlobalData gData = GlobalData::Get();

	float ratio = (float)y / (float)x;
	m_HUDpos = { (1.0f / ratio) - 0.2f, -0.8f };
}

void FiberWire::ClearResources()
{
	m_texHUD = 0;
	m_texInGame = 0;
}
