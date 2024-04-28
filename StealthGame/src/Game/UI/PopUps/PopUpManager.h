#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include "../../Desc.h"

#include "../../GlobalData.h"

#include "../../../Scene.h"

#include "PopUp.h"

class PopUpManager
{
public:
	void ClearResources();

	void AddPopUp(GameTickDesc& desc, PopUp& popUp);
	void AddDelete(uint64_t id);
	void DeletePopUp(GameTickDesc& desc, uint64_t id);

	void UpdatePopUps(GameTickDesc& desc);
private:
	std::unordered_map<uint64_t, PopUp> m_popUps;
	std::vector<uint64_t> m_deletes;
};