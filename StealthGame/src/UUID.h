#pragma once

#include <random>

class UUID
{
public:
	UUID();
	UUID(uint64_t& uuid);

	const uint64_t& GetUUID() { return m_uuid; }
private:
	uint64_t m_uuid;
};