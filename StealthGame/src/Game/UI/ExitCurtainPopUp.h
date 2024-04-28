#pragma once

#include "PopUps/PopUpManager.h"

#undef min
#undef max

class ExitCurtainPopUp
{
public:
	void ClearResources();

	void StartEnd(GameTickDesc& desc, PopUpManager* manager);
	void StartStart(GameTickDesc& desc, PopUpManager* manager);
private:
	PopUpManager* m_manager = nullptr;
	PopUp m_startPopUp;
	PopUp m_endPopUp;

	bool m_endStarted = false;
	bool m_startStarted = false;
};