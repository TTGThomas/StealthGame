#include "Projectile.h"

Projectile::Projectile(ProjectileInitDesc desc)
{
	GlobalData& gData = GlobalData::Get();

	//m_audio = gData.m_scene->GetAudio().AddSound(
	//	GameUUID(gData.m_audioGun2),
	//	pos,
	//	5.0f, 7.0f,
	//	true,
	//	false
	//).GetUUID();
	gData.m_scene->GetAudio().StartSound(desc.m_audioUUID);

	m_lifeStart = (float)glfwGetTime();

	m_vel = { glm::sin(glm::radians(desc.m_rot)), glm::cos(glm::radians(desc.m_rot)) };
	m_pos = desc.m_pos;

	std::vector<QuadInitDesc> descs;
	float size = desc.m_size;
	descs.push_back({ m_pos, { size, size }, 0.9f, gData.m_defaultShader, desc.m_texID });
	descs.push_back({ m_pos, { size, size }, 0.9f, gData.m_defaultShader, desc.m_texID });
	m_entity.Init(descs);
	gData.m_collision->AddToLayer(desc.m_colLayer, m_entity.GetUUID(0).GetUUID());
	
	m_speed = desc.m_speed;
	m_lifeSpan = desc.m_lifeSpan;
}

bool Projectile::Tick()
{
	// returns if it is alive or not
	GlobalData& gData = GlobalData::Get();

	bool ret = true;

	if ((float)glfwGetTime() - m_lifeStart > m_lifeSpan)
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