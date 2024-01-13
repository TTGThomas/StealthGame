#pragma once

#include <glm/glm.hpp>

class Interaction
{
public:
	virtual void OnInteract() {}
	virtual glm::vec2 OnTick() { return {}; } // return where you want your cursor to be at
};