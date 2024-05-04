#pragma once

#include <glm/glm.hpp>

#include "Interaction.h"

#include "../Engine/Object.h"

#include "../GlobalData.h"

class FoodInteract : public Interaction
{
public:
	FoodInteract(class GameScene* scene, int objectIndex)
		: m_scene(scene), m_objIndex(objectIndex) {}

	virtual void OnInteract() override;

	// returns where you want the cursor to be at
	virtual glm::vec2 OnTick(bool* show) override;

	virtual Interaction::Type GetType() override { return Interaction::Type::FOOD; }
	bool IsPoisoned() { return m_poisoned; }
private:
	GameScene* m_scene = nullptr;
	int m_objIndex = -1;

	bool m_poisoned = false;
};