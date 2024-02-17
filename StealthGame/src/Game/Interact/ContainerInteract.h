#pragma once

#include "Interaction.h"

#include "../Engine/Object.h"

class ContainerInteract : public Interaction
{
public:
	ContainerInteract(class GameScene* scene, int objIndex)
		: m_scene(scene), m_objIndex(objIndex) {}

	virtual void OnInteract() override;

	// returns where you want the cursor to be at
	virtual glm::vec2 OnTick() override;
private:
	int m_objIndex = -1;
	GameScene* m_scene = nullptr;
	int m_spaceLeft = 2;
};