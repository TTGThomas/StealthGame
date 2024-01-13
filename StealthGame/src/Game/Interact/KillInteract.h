#pragma once

#include "Interaction.h"

#include "../Characters/Player.h"
#include "../Characters/NPC.h"

class KillInteract : public Interaction
{
public:
	KillInteract(Player* player, NPC* npc)
		: m_player(player), m_npc(npc) {}

	virtual void OnInteract() override;
	virtual glm::vec2 OnTick() override;
private:
	Player* m_player = nullptr;
	NPC* m_npc = nullptr;
};

