#pragma once

#include <functional>

#include "glm/glm.hpp"

#include "Entity.h"

#include "../GlobalData.h"
#include "../Desc.h"

struct ProjectileInitDesc
{
	glm::vec2 m_pos;
	int m_colLayer;
	float m_rot;
	float m_speed;
	float m_size;
	float m_lifeSpan;
	uint64_t m_texID;
	uint64_t m_audioUUID;
};

class Projectile
{
public:
	Projectile(ProjectileInitDesc desc);

	bool Tick();
private:
	uint64_t m_audio = 0;
	Entity m_entity;
	float m_lifeStart;
	float m_lifeSpan = 0.0f;

	glm::vec2 m_pos;
	glm::vec2 m_vel;

	float m_speed;
};