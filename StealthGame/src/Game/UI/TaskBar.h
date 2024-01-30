#pragma once

#include <vector>

#include "../Desc.h"

#include "../GlobalData.h"

class TaskBar
{
public:
	void Init(GameTickDesc& desc);

	void SetTargets(std::vector<uint64_t>& target) { m_targetNPCs = target; }

	void ShowTaskBar();
private:
	std::vector<uint64_t> m_targetNPCs;
};