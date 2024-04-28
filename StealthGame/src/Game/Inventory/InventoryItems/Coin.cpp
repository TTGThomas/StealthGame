#include "Coin.h"

#include "../../GameScene.h"

uint64_t Coin::m_texHUD;
uint64_t Coin::m_texInGame;

void Coin::OnEquip()
{
	GlobalData gData = GlobalData::Get();

	if (m_texHUD == 0)
		m_texHUD = CreateTexture("res/Inventories/coinHUD.png");
	if (m_texInGame == 0)
		m_texInGame = CreateTexture("res/Inventories/coin.png");
	AddQuad({}, { 0.2f, 0.2f }, false, m_texHUD);
	AddQuad({}, { 0.2f, 0.2f }, true, m_texInGame);
}

void Coin::OnUnequip()
{
	GlobalData gData = GlobalData::Get();

	gData.m_scene->DeleteQuad(m_uuids[0].GetUUID());
	gData.m_scene->DeleteQuad(m_uuids[1].GetUUID());
	m_uuids = {};
}

void Coin::OnEquipping()
{
	GlobalData gData = GlobalData::Get();
	Player& player = gData.m_gameScene->GetPlayer();

	gData.m_scene->GetQuads()[m_uuids[0].GetUUID()].SetPos(m_HUDpos);

	{
		glm::vec2 pos = gData.m_gameScene->GetPlayer().GetPos();
		gData.m_scene->GetQuads()[m_uuids[1].GetUUID()].SetPos(pos);
	}

	glm::vec2 mousePos = { Mouse::GetMouseX(), Mouse::GetMouseY() };
	mousePos = mousePos / m_windowScale * 2.0f - 1.0f;
	mousePos.x *= 1.0f / (m_windowScale.y / m_windowScale.x);
	mousePos.y = -mousePos.y;

	float rotation = AngleFromPoint(player.GetPos(), player.GetPos() + mousePos);

	if (Mouse::IsMousePressDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		// fire projectile
		uint64_t t = gData.m_scene->GetAudio().AddSound(
			GameUUID(gData.m_audioGun1),
			player.GetPos(),
			5.0f, 7.0f,
			true,
			false
		).GetUUID();
		ProjectileInitDesc desc;
		desc.m_audioUUID = t;
		desc.m_colLayer = 4;
		desc.m_lifeSpan = 5.0f;
		desc.m_pos = player.GetPos();;
		desc.m_rot = rotation;
		desc.m_size = 0.05f;
		desc.m_speed = 10.0f;
		desc.m_texID = gData.m_texBullet;
		gData.m_gameScene->GetProjectiles().emplace_back(desc);
	}
}

void Coin::OnResize(int x, int y)
{
	GlobalData gData = GlobalData::Get();

	m_windowScale = { x, y };
	float ratio = (float)y / (float)x;
	m_HUDpos = { (1.0f / ratio) - 0.2f, -0.8f };
}

bool Coin::IsIllegal()
{
	return false;
}

void Coin::ClearResources()
{
	m_texHUD = 0;
	m_texInGame = 0;
}

float Coin::AngleFromPoint(glm::vec2 start, glm::vec2 end)
{
	end = glm::normalize(end - start);
	glm::vec2 front = glm::vec2(0.0f, 1.0f);

	float degree = glm::degrees(glm::acos(glm::dot(end, front)));
	return (end.x > 0.0f ? degree : 360.0f - degree);
}
