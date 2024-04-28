#include "PopUpManager.h"

void PopUpManager::ClearResources()
{
	m_popUps.clear();
}

void PopUpManager::AddPopUp(GameTickDesc& desc, PopUp& popUp)
{
	popUp.OnCreate(desc);
	m_popUps[popUp.GetPopUUID().GetUUID()] = popUp;
}

void PopUpManager::AddDelete(uint64_t id)
{
	m_deletes.emplace_back(id);
}

void PopUpManager::DeletePopUp(GameTickDesc& desc, uint64_t id)
{
	m_popUps[id].OnDelete(desc);
	m_popUps.erase(id);
}

void PopUpManager::UpdatePopUps(GameTickDesc& desc)
{
	for (auto& [uuid, popUp] : m_popUps)
		popUp.OnTick(desc);

	for (uint64_t id : m_deletes)
		DeletePopUp(desc, id);
	m_deletes = {};
}