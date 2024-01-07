#pragma once

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
	Disguise(Type type)
		: m_type(type) {}

	void Init(Type type) { m_type = type; }

	virtual void OnCollect(Player* player) override;

	void SetType(Type type) { m_type = type; }

	Type GetType() { return m_type; }
private:
	Type m_type;
};

