#include "NPC.h"

void NPC::NPCTick(GameTickDesc& desc)
{
	if (Mouse::IsMousePressDown(GLFW_MOUSE_BUTTON_MIDDLE))
		m_dir++;
	SetPos(GetPos());

	while (m_dir > 7)
		m_dir -= 8;
}

void NPC::SetPos(glm::vec2 newPos)
{
	GetQuad(0)->SetPos(newPos);
	GetQuad(2)->SetPos(newPos);
	m_map->GetQuads()[GetIndex(1)].SetPos(newPos + m_dirOffset[m_dir] * 0.2f);
}

void NPC::ChangePos(glm::vec2 pos)
{
	GetQuad(0)->ChangePos(pos);
	GetQuad(2)->ChangePos(pos);
	m_map->GetQuads()[GetIndex(1)].SetPos(GetQuad(0)->GetPos() + m_dirOffset[m_dir] * 0.2f);
}

void NPC::EliminateMyself()
{
}