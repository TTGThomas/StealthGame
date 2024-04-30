#include "Gun.h"

#include "../../GameScene.h"

uint64_t Gun::m_texHUD;
uint64_t Gun::m_texInGame;

void Gun::OnEquip()
{
	GlobalData gData = GlobalData::Get();

	if (m_texHUD == 0)
		m_texHUD = CreateTexture("res/Inventories/GunHUD.png");
	if (m_texInGame == 0)
		m_texInGame = CreateTexture("res/Inventories/Gun.png");
	AddQuad({}, { MAP_RADIUS, MAP_RADIUS }, false, m_texHUD);
	AddQuad({}, { MAP_RADIUS, MAP_RADIUS }, true, m_texInGame);
}

void Gun::OnUnequip()
{
	GlobalData gData = GlobalData::Get();

	gData.m_scene->DeleteQuad(m_uuids[0].GetUUID());
	gData.m_scene->DeleteQuad(m_uuids[1].GetUUID());
	m_uuids = {};
}

void Gun::OnEquipping()
{
	m_isShooting = false;
	GlobalData gData = GlobalData::Get();

	gData.m_scene->GetQuads()[m_uuids[0].GetUUID()].SetPos(m_HUDpos);

	Player& player = gData.m_gameScene->GetPlayer();

	glm::vec2 mousePos = { Mouse::GetMouseX(), Mouse::GetMouseY() };
	mousePos = mousePos / m_windowScale * 2.0f - 1.0f;
	mousePos.x *= 1.0f / (m_windowScale.y / m_windowScale.x);
	mousePos.y = -mousePos.y;
	float rotation = AngleFromPoint(player.GetPos(), player.GetPos() + mousePos);
	gData.m_scene->GetQuads()[GetUUID(1)].SetRotation(rotation);

	glm::vec2 pos = player.GetPos();
	glm::vec2 front = { glm::sin(glm::radians(rotation)), glm::cos(glm::radians(rotation)) };
	pos += 0.15f * front;
	float pixelSize = 0.2f / 16.0f;
	if (rotation > 180.0f)
		gData.m_scene->GetQuads()[GetUUID(1)].SetRadius({ -0.1f, 0.1f });
	else
		gData.m_scene->GetQuads()[GetUUID(1)].SetRadius({ 0.1f, 0.1f });
	gData.m_scene->GetQuads()[GetUUID(1)].SetPos(pos);

	if (Mouse::IsMousePressDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		// fire projectile
		uint64_t t = gData.m_scene->GetAudio().AddSound(
			GameUUID(gData.m_audioGun1),
			pos,
			1.0f, 7.0f,
			true,
			false
		).GetUUID();
		ProjectileInitDesc desc;
		desc.m_audioUUID = t;
		desc.m_colLayer = 2;
		desc.m_lifeSpan = 5.0f;
		desc.m_pos = pos;
		desc.m_rot = rotation;
		desc.m_size = 0.05f;
		desc.m_speed = 10.0f;
		desc.m_texID = gData.m_texBullet;
		gData.m_gameScene->GetProjectiles().emplace_back(std::make_unique<Projectile>(desc));
		m_isShooting = true;
	}
}

void Gun::OnResize(int x, int y)
{
	GlobalData gData = GlobalData::Get();

	m_windowScale = { x, y };

	float ratio = (float)y / (float)x;
	m_HUDpos = { (1.0f / ratio) - 0.2f, -0.8f };
}

bool Gun::IsIllegal()
{
	GlobalData& gData = GlobalData::Get();
	Player& player = gData.m_gameScene->GetPlayer();
	return player.GetDisguise() < Identities::GUARD;
}

void Gun::ClearResources()
{
	m_texHUD = 0;
	m_texInGame = 0;
}

float Gun::AngleFromPoint(glm::vec2 start, glm::vec2 end)
{
	end = glm::normalize(end - start);
	glm::vec2 front = glm::vec2(0.0f, 1.0f);

	float degree = glm::degrees(glm::acos(glm::dot(end, front)));
	return (end.x > 0.0f ? degree : 360.0f - degree);
}
