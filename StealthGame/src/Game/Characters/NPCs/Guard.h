#pragma once

#include "../NPC.h"

// GUARD:
// 
//       +------------+
//       |SearchPlayer|
//       +------------+
//          ^      |
//          |      v
// +-----+->+------+ +------------+
// |Alert|  |Attack| |SeachGunShot|
// +-----+<-+------+ +------------+
//  ^    |             ^      |
//  |    v             |      v
// +-----------+------>+------+->+---------+
// |MoveOnRoute|       |Search|  |SeachBody|
// +-----------+<------+------+<-+---------+
//    ^      |         ^      |
//    |      v         |      v
// +------------+      +------------+
// |LookToPlayer|      |SeachIllegal|
// +------------+      +------------+

class Guard : public NPC
{
public:
protected:
	virtual void InitNodeGraph() override;
private:
	glm::vec2 m_searchPos = {};
	void* m_searchParam = nullptr;// can be anything
	bool m_searchFinish = false;
	NPC::ReportSearch m_reportedSearch = ReportSearch::NONE;

	float m_shootDur = 0.0f;
};