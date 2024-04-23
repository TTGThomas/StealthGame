#include "Projectile.h"

Projectile::Projectile(glm::vec2 pos, int layer, float rot, uint64_t& texID)
{
	GlobalData& gData = GlobalData::Get();

	m_audio = gData.m_scene->GetAudio().AddSound(
		GameUUID(gData.m_audioGun1),
		pos,
		5.0f, 7.0f,
		true,
		false
	).GetUUID();
	gData.m_scene->GetAudio().StartSound(m_audio);

	m_lifeStart = (float)glfwGetTime();

	m_vel = { glm::sin(glm::radians(rot)), glm::cos(glm::radians(rot)) };
	m_pos = pos + m_vel * 0.2f;

	std::vector<QuadInitDesc> descs;
	float size = 0.05f;
	descs.push_back({ m_pos, { size, size }, 0.9f, gData.m_defaultShader, texID });
	descs.push_back({ m_pos, { size, size }, 0.9f, gData.m_defaultShader, texID });
	m_entity.Init(descs);
	gData.m_collision->AddToLayer(layer, m_entity.GetUUID(0).GetUUID());
	
	m_speed = 10.0f;
}

bool Projectile::Tick()
{
	// returns if it is alive or not
	GlobalData& gData = GlobalData::Get();

	bool ret = true;

	if ((float)glfwGetTime() - m_lifeStart > 5.0f)
		ret = false;
	if (gData.m_collision->Collide(0, m_entity.GetUUID(0)).m_hasHit)
		ret = false;

	m_entity.GetQuad(0)->ChangePos(m_vel * m_speed * gData.m_deltaTime);
	m_entity.GetQuad(1)->ChangePos(m_vel * m_speed * gData.m_deltaTime);

	gData.m_scene->GetAudio().SetSoundPos(m_audio, m_entity.GetQuad(0)->GetPos());

	if (!ret)
	{
		gData.m_scene->DeleteQuad(m_entity.GetUUID(0).GetUUID());
		gData.m_scene->DeleteQuad(m_entity.GetUUID(1).GetUUID());
	}
	return ret;
}