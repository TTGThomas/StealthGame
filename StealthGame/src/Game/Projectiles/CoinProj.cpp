#include "CoinProj.h"

#include "../SceneLoader/SceneLoader.h"

CoinProj::CoinProj(ProjectileInitDesc& desc, glm::vec2 pos)
{
	GlobalData& gData = GlobalData::Get();

	m_audio = desc.m_audioUUID;

	m_lifeStart = (float)glfwGetTime();

	m_vel = { glm::sin(glm::radians(desc.m_rot)), glm::cos(glm::radians(desc.m_rot)) };
	m_pos = desc.m_pos;
	m_targetPos = pos;

	std::vector<QuadInitDesc> descs;
	float size = desc.m_size;
	descs.push_back({ m_pos, { size, size }, SceneLoader::Depth(0.991f), gData.m_defaultShader, desc.m_texID });
	descs.push_back({ m_pos, { size, size }, SceneLoader::Depth(0.991f), gData.m_defaultShader, desc.m_texID });
	m_entity.Init(descs);
	gData.m_collision->AddToLayer(desc.m_colLayer, m_entity.GetUUID(0).GetUUID());

	m_speed = desc.m_speed;
	m_lifeSpan = desc.m_lifeSpan;
}

bool CoinProj::Tick()
{
	// returns if it is alive or not
	GlobalData& gData = GlobalData::Get();

	bool ret = true;

	float time = (float)glfwGetTime() - m_lifeStart;
	if (time > m_lifeSpan + 3.0f)
		ret = false;

	// h: max height
	// t: time
	// x: time
	// y: z axis of coin

	// y = (-4hx^2 + 4hxt) / t^2
	glm::vec3 pos{};
	float percentToDie = glm::min(1.0f, time / m_lifeSpan);
	pos = glm::vec3((1.0f - percentToDie) * m_pos + percentToDie * m_targetPos, 0.0f);

	float h = 1.0f;
	float t = m_lifeSpan;
	float x = time;
	pos.z = std::max((-4.0f * h * time * time + 4.0f * h * x * t) / (t * t), 0.0f);

	m_entity.GetQuad(0)->SetPos({ pos.x, pos.y + pos.z });
	m_entity.GetQuad(1)->SetPos({ pos.x, pos.y + pos.z });

	if (time > m_lifeSpan && !m_played)
	{
		m_played = true;
		gData.m_scene->GetAudio().StartSound(m_audio);
	}

	if (!ret)
	{
		gData.m_scene->DeleteQuad(m_entity.GetUUID(0).GetUUID());
		gData.m_scene->DeleteQuad(m_entity.GetUUID(1).GetUUID());
	}
	return ret;
}
