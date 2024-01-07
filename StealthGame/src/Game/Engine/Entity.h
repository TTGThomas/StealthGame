#pragma once

#include "../../Collisions/CollisionDetector.h"

#include "Object.h"

// first in desc list acts as the hitbox for the whole entity
class Entity : public Object
{
public:
	void Move(CollisionDetector* collision, float x, float y);
};