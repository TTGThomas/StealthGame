#pragma once

#include "../Engine/Object.h"

#include "../Characters/Player.h"
#include "../Characters/NPC.h"

#include "../../quadRenderer/QuadRenderer.h"

#include "../GameScene.h"

#include "Item.h"

#include "../GlobalData.h"

class Disguise : public Item
{
public:
	Disguise() = default;

	void Init(Identities type, glm::vec2 pos, float depth, uint64_t shaderUUID);

	virtual void OnCollect(class GameScene* scene) override;

	Identities GetType() { return m_type; }
private:
	uint64_t DisguiseFromIdentity(Identities type);
	uint64_t TexFromIdentity(Identities type);
private:
	Identities m_type;
};

