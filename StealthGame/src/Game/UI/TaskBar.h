#pragma once

#include <vector>

#include "../Desc.h"

#include "../GlobalData.h"

class TaskBar
{
public:
	void Init(GameTickDesc& desc);

	void SetTargets(std::vector<uint64_t>& target) { m_targetNPCs = target; }

	void ShowTaskBar(GameTickDesc& desc);
private:
	std::vector<uint64_t> m_targetNPCs;
};