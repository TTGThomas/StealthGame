#include "NPCStats.h"

int NPCStats::m_guestTextureIndex;
int NPCStats::m_vipGuestTextureIndex;
int NPCStats::m_guardTextureIndex;
int NPCStats::m_vipGuardTextureIndex;

int NPCStats::m_deadBodyTextureIndex;

void NPCStats::SetTextures(NPCStatInitDesc& desc)
{
	m_guestTextureIndex = desc.m_guestTextureIndex;
	m_vipGuestTextureIndex = desc.m_vipGuestTextureIndex;
	m_guardTextureIndex = desc.m_guardTextureIndex;
	m_vipGuardTextureIndex = desc.m_vipGuardTextureIndex;
	m_deadBodyTextureIndex = desc.m_deadBodyTextureIndex;
}