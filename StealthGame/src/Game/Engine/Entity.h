#pragma once

#include "../../Collisions/CollisionDetector.h"

#include "Object.h"

#include "../GlobalData.h"
#include "../../Scene.h"

// first in desc list acts as the hitbox for the whole entity
class Entity : public Object
{
public:
	virtual void Init(std::vector<QuadInitDesc>& descs, bool isSpecial = false, bool isDoor = false) override;

	void Move(CollisionDetector* collision, int layer, float x, float y);
};