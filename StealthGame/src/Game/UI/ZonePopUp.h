#pragma once

#include "PopUps/PopUpManager.h"

class ZonePopUp
{
public:
	enum class State
	{
		NORMAL, TRESPASSING, HOSTILE
	};
public:
	void Init(GameTickDesc& desc, PopUpManager* manager);

	void ClearResources();

	void SetStartPos(glm::vec2 pos);

	void OnTrespass();
	void OnHostile();

	void OnExit(GameTickDesc& desc);

	float GetFontSize();
private:
	PopUpManager* m_manager;
	PopUp m_popUp;
};

