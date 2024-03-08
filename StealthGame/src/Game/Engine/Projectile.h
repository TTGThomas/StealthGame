#pragma once

#include <functional>

#include "glm/glm.hpp"

#include "Entity.h"

#include "../GlobalData.h"

class Projectile
{
public:
	Projectile(glm::vec2 pos, float rot, uint64_t& texID);

	bool Tick();
private:
	Entity m_entity;
	float m_lifeStart;

	glm::vec2 m_pos;
	glm::vec2 m_vel;

	float m_speed;
};