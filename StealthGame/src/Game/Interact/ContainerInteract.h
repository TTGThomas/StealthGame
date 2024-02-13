#pragma once

#include "Interaction.h"

#include "../Engine/Object.h"

class ContainerInteract : public Interaction
{
public:
	ContainerInteract(class GameScene* scene, Object* object)
		: m_scene(scene), m_obj(object) {}

	virtual void OnInteract() override;

	// returns where you want the cursor to be at
	virtual glm::vec2 OnTick() override;
private:
	Object* m_obj = nullptr;
	GameScene* m_scene = nullptr;
};