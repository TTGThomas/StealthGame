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
	enum class Type
	{
		STANDARD, GUEST, VIPGUEST, GAURD, VIPGAURD
	};
public:
	Disguise() = default;

	void Init(Type type, glm::vec2 pos, float depth, int shaderIndex);

	virtual void OnCollect(class GameScene* scene) override;

	Type GetType() { return m_type; }
private:
	Type m_type;
};

