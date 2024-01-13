#pragma once

class NPCStats
{
public:
	struct NPCStatInitDesc
	{
		int m_guestTextureIndex;
		int m_vipGuestTextureIndex;
		int m_guardTextureIndex;
		int m_vipGuardTextureIndex;

		int m_deadBodyTextureIndex;
	};
public:
	static void SetTextures(NPCStatInitDesc& desc);
private:
	static int m_guestTextureIndex;
	static int m_vipGuestTextureIndex;
	static int m_guardTextureIndex;
	static int m_vipGuardTextureIndex;

	static int m_deadBodyTextureIndex;
};