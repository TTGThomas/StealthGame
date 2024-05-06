#pragma once

#include "Interaction.h"

#include "../Items/Item.h"

#include "../Engine/Object.h"

#include "../GlobalData.h"

class ExitInteract : public Interaction
{
public:
	ExitInteract(class GameScene* scene, class Game* game, int objIndex)
		: m_scene(scene), m_objIndex(objIndex), m_game(game) {}

	virtual void OnInteract() override;

	// returns where you want the cursor to be at
	virtual glm::vec2 OnTick(bool* show) override;

	virtual Interaction::Type GetType() override { return Interaction::Type::EXIT; }
private:
	Game* m_game = nullptr;
	GameScene* m_scene = nullptr;
	int m_objIndex = -1;
};

