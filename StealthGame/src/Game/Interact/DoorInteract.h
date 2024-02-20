#pragma once

#include <glm/glm.hpp>

#include "Interaction.h"

#include "../Engine/Object.h"

#include "../GlobalData.h"

class DoorInteract : public Interaction
{
public:
	DoorInteract(class GameScene* scene, int objIndex, glm::vec2 radius)
		: m_scene(scene), m_objIndex(objIndex), m_radius(radius) {}

	virtual void OnInteract() override;

	// returns where you want the cursor to be at
	virtual glm::vec2 OnTick(bool* show) override;
private:
	int m_objIndex = -1;
	GameScene* m_scene = nullptr;
	bool m_isOpen = false;

	glm::vec2 m_radius = {};
};