#include "PopUpManager.h"

void PopUpManager::AddPopUp(GameTickDesc& desc, PopUp& popUp)
{
	m_popUps[popUp.GetPopUUID().GetUUID()] = popUp;
	popUp.OnCreate(desc);
}

void PopUpManager::DeletePopUp(GameTickDesc& desc, uint64_t id)
{
	m_popUps[id].OnDelete(desc);
	m_popUps.erase(id);
}

void PopUpManager::UpdatePopUps(GameTickDesc& desc)
{
	for (auto& [uuid, popUp] : m_popUps)
	{
		popUp.OnTick(desc);
	}
}