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
	bool m_routeFinished = true;
	bool m_searchFinish = false;
	uint64_t m_reportNPC = 0;
	glm::vec2 m_reportPos = {};
	Identities m_reportIdentity = Identities::STANDARD;
	int m_reportIdentityVal = false;
};