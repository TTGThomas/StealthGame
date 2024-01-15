#pragma once

struct NPCStatInitDesc
{
	int m_guestTextureIndex;
	int m_vipGuestTextureIndex;
	int m_guardTextureIndex;
	int m_vipGuardTextureIndex;

	int m_deadBodyTextureIndex;
};

class NPCStats
{
public:
	static void SetTextures(NPCStatInitDesc& desc);

	static int GetGuestTextureIndex() { return m_guestTextureIndex; }
	static int GetVIPGuestTextureIndex() { return m_vipGuestTextureIndex; }
	static int GetGuardTextureIndex() { return m_guardTextureIndex; }
	static int GetVIPGuardTextureIndex() { return m_vipGuardTextureIndex; }
	static int GetDeadBodyTextureIndex() { return m_deadBodyTextureIndex; }
private:
	static int m_guestTextureIndex;
	static int m_vipGuestTextureIndex;
	static int m_guardTextureIndex;
	static int m_vipGuardTextureIndex;

	static int m_deadBodyTextureIndex;
};