#include "Gun.h"

#include "../../GameScene.h"

uint64_t Gun::m_texHUD;
uint64_t Gun::m_texInGame;
std::vector<Projectile> Gun::m_projectiles;

void Gun::OnEquip()
{
	GlobalData gData = GlobalData::Get();

	if (m_texHUD == 0)
		m_texHUD = CreateTexture("res/Inventories/GunHUD.png");
	if (m_texInGame == 0)
		m_texInGame = CreateTexture("res/Inventories/Gun.png");
	AddQuad({}, { 0.2f, 0.2f }, false, m_texHUD);
	AddQuad({}, { 0.2f, 0.2f }, true, m_texInGame);
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
	GlobalData gData = GlobalData::Get();

	gData.m_scene->GetQuads()[m_uuids[0].GetUUID()].SetPos(m_HUDpos);

	Player& player = gData.m_gameScene->GetPlayer();

	glm::vec2 mousePos = { Mouse::GetMouseX(), Mouse::GetMouseY() };
	mousePos = mousePos / m_windowScale * 2.0f - 1.0f;
	mousePos.x *= 1.0f / (m_windowScale.y / m_windowScale.x);
	mousePos.y = -mousePos.y;
	float rotation = AngleFromPoint(player.GetPos(), player.GetPos() + mousePos);
	gData.m_scene->GetQuads()[m_uuids[1].GetUUID()].SetRotation(rotation);

	glm::vec2 pos = player.GetPos();
	glm::vec2 front = { glm::sin(glm::radians(rotation)), glm::cos(glm::radians(rotation)) };
	pos += 0.4f * front;
	gData.m_scene->GetQuads()[m_uuids[1].GetUUID()].SetPos(pos);

	if (Mouse::IsMousePressDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		// fire projectile
		m_projectiles.emplace_back(player.GetPos(), rotation, gData.m_texBullet);
	}
}

void Gun::OnResize(int x, int y)
{
	GlobalData gData = GlobalData::Get();

	m_windowScale = { x, y };

	float ratio = (float)y / (float)x;
	m_HUDpos = { (1.0f / ratio) - 0.2f, -0.8f };
}

void Gun::ProjectileTick()
{
	for (int i = 0; i < m_projectiles.size(); )
	{
		Projectile& proj = m_projectiles[i];
		if (!proj.Tick())
			m_projectiles.erase(m_projectiles.begin() + i);
		else
			i++;
	}
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
