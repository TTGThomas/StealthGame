#pragma once

#include "Interaction.h"

#include "../Items/Item.h"

#include "../Engine/Object.h"

#include "../GlobalData.h"

class ExitInteract : public Interaction
{
public:
	ExitInteract(class GameScene* scene, class Game* game, int objIndex, int levelIndex)
		: m_scene(scene), m_objIndex(objIndex), m_game(game), m_levelIndex(levelIndex) {}

	virtual void OnInteract() override;

	// returns where you want the cursor to be at
	virtual glm::vec2 OnTick(bool* show) override;
private:
	Game* m_game = nullptr;
	GameScene* m_scene = nullptr;
	int m_objIndex = -1;
	int m_levelIndex = -2;
};

