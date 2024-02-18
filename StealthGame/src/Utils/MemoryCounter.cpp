#include "MemoryCounter.h"

#define ENABLEMEMORYCOUNTER

std::vector<char> MemoryCounter::m_buffer;
size_t MemoryCounter::m_allocate = 0;
size_t MemoryCounter::m_free = 0;

#ifdef ENABLEMEMORYCOUNTER
void* operator new(size_t size)
{
	MemoryCounter::AddAllocate(size);
	return malloc(size);
}

void operator delete(void* block, size_t size)
{
	MemoryCounter::AddFree(size);
	free(block);
}
#endif

void MemoryCounter::ShowStatsWindow()
{
#ifdef ENABLEMEMORYCOUNTER
	ImGui::Begin("MemoryCounter");

	AddLog("Allocated: %u | freed: %u | unfreed: %u", m_allocate, m_free, m_allocate - m_free);

	if (!m_buffer.empty())
		ImGui::TextUnformatted(&m_buffer[0], &m_buffer[m_buffer.size() - 1]);

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);
	ImGui::End();
#endif
}

// text must be no longer than 512 characters
void MemoryCounter::AddLog(const char* text, ...)
{
#ifdef ENABLEMEMORYCOUNTER
	va_list args;
	va_start(args, text);
	char buffer[512];
	int len = _vsnprintf_s(buffer, 512, text, args);
	m_buffer.reserve(m_buffer.size() + len);
	for (int i = 0; i < len; i++)
		m_buffer.emplace_back(buffer[i]);
	m_buffer.emplace_back('\n');
	va_end(args);
#endif
}