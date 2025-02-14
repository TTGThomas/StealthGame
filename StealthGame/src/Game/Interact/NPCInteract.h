#pragma once

#include "Interaction.h"

#include "../Characters/Player.h"
#include "../Characters/NPC.h"

#include "../GameScene.h"

class NPCInteract : public Interaction
{
public:
	NPCInteract(GameScene* gameScene, NPC* npc)
		: m_gameScene(gameScene), m_npc(npc) {}

	virtual void OnInteract() override;
	virtual glm::vec2 OnTick(bool* show) override;

	virtual Interaction::Type GetType() override { return Interaction::Type::NPC; }
private:
	GameScene* m_gameScene = nullptr;
	NPC* m_npc = nullptr;
};

