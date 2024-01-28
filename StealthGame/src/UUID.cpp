#include "UUID.h"

static std::random_device randomDevice;
static std::mt19937_64 randomEngine(randomDevice());
static std::uniform_int_distribution<uint64_t> uniformDistribution;

UUID::UUID()
	: m_uuid(uniformDistribution(randomEngine))
{
}

UUID::UUID(uint64_t& uuid)
	: m_uuid(uuid)
{
}

UUID::UUID(const uint64_t& uuid)
	: m_uuid(uuid)
{
}

UUID::UUID(uint64_t&& uuid)
	: m_uuid(uuid)
{
}

void UUID::RegenerateUUID()
{
	m_uuid = uniformDistribution(randomEngine);
}