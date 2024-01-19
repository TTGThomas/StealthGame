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
	void SetUUID(UUID uuid) { m_uuid = uuid; }

	virtual void OnCollect(class GameScene* scene) {}

	Quad& GetQuad() { return GlobalData::Get().m_scene->GetQuads()[m_uuid.GetUUID()]; }
	UUID& GetUUID() { return m_uuid; }
protected:
	UUID m_uuid;
};