#pragma once

#include <glm/glm.hpp>

class Interaction
{
public:
	enum class Type
	{
		NONE, CONTAINER, DOOR, EXIT, FOOD, ITEM, NPC
	};
public:
	virtual void OnInteract() {}
	// bool* show : indicates if you want to show the cursor or not
	virtual glm::vec2 OnTick(bool* show) { return {}; } // return where you want your cursor to be at
	virtual Type GetType() { return Type::NONE; }
};