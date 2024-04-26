#pragma once

#include "../NPC.h"

// GUEST:
//
// +-----------+-->+------+->+-------------+
// |MoveOnRoute|   |Search|  |ReportToGuard|
// +-----------+<--+------+<-+-------------+
//    ^      |   
//    |      v   
// +------------+
// |LookToPlayer|
// +------------+

class Guest : public NPC
{
public:
protected:
	virtual void InitNodeGraph() override;
private:
	glm::vec2 m_searchPos = {};
	void* m_searchParam = nullptr;// can be anything
	bool m_searchFinish = false;
};