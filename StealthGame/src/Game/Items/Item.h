#pragma once

#include "../Engine/Object.h"

#include "../Characters/Player.h"
#include "../Characters/NPC.h"

// haves no hitbox
// every quad acts as a character
class Item : public Object
{
public:
	virtual void Init(QuadRenderer* map, std::vector<QuadInitDesc>& descs) override;

	virtual void OnCollect(Player* player) {}
	virtual void OnCollect(NPC* npc) {}
};
