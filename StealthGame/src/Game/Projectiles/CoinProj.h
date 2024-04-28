#pragma once

#include "../Engine/Projectile.h"

#undef max

class CoinProj : public Projectile
{
public:
	CoinProj(ProjectileInitDesc& desc, glm::vec2 pos);

	virtual bool Tick();
private:
	glm::vec2 m_targetPos;
	bool m_played = false;
};