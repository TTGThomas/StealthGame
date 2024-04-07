#pragma once

#include "../../quadRenderer/QuadRenderer.h"

#include "../../quadRenderer/RenderQuad.h"

#include "../../Scene.h"

#include "../GlobalData.h"

#include "../../UUID.h"

// items should only have one quad
class Item
{
public:
	void SetUUID(GameUUID uuid) { m_uuid = uuid; }

	virtual void OnCollect(class GameScene* scene) {}

	virtual bool IsIllegal() { return false; }

	Quad& GetQuad() { return GlobalData::Get().m_scene->GetQuads()[m_uuid.GetUUID()]; }
	GameUUID& GetUUID() { return m_uuid; }
protected:
	GameUUID m_uuid;
};