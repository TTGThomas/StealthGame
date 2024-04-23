#pragma once

#include <functional>

#include "glm/glm.hpp"

#include "Entity.h"

#include "../GlobalData.h"
#include "../Desc.h"

class Projectile
{
public:
	Projectile(glm::vec2 pos, int layer, float rot, uint64_t& texID);

	bool Tick();
private:
	uint64_t m_audio = 0;
	Entity m_entity;
	float m_lifeStart;

	glm::vec2 m_pos;
	glm::vec2 m_vel;

	float m_speed;
};