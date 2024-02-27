#pragma once

#include "../../../input/KeyBoard.h"
#include "../../../input/Mouse.h"

#include "InventoryItem.h"

class FiberWire : public InventoryItem
{
public:
	virtual void OnEquip() override;
	virtual void OnUnequip() override;
	virtual void OnEquipping() override;
private:
};

