#pragma once

#include "../Engine/Object.h"

#include "../Characters/Player.h"
#include "../Characters/NPC.h"

#include "../../quadRenderer/QuadRenderer.h"

#include "../Scene.h"

#include "Item.h"

class Disguise : public Item
{
public:
	enum class Type
	{
		STANDARD, GUEST, VIPGUEST, GAURD, VIPGAURD
	};
public:
	Disguise() = default;

	void Init(QuadRenderer* renderer, Type type, glm::vec2 pos, int shaderIndex);

	virtual void OnCollect(class Scene* scene) override;

	Type GetType() { return m_type; }
private:
	Type m_type;
};

