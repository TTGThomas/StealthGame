#pragma once

#include "../../Collisions/CollisionDetector.h"

#include "Object.h"

#include "../GlobalData.h"
#include "../../Scene.h"

// first in desc list acts as the hitbox for the whole entity
class Entity : public Object
{
public:
	virtual void Init(std::vector<QuadInitDesc>& descs) override;

	void Move(CollisionDetector* collision, float x, float y);
};