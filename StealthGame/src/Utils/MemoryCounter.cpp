#include "MemoryCounter.h"

//#define ENABLEMEMORYCOUNTER

std::vector<char> MemoryCounter::m_buffer;
size_t MemoryCounter::m_allocate = 0;
size_t MemoryCounter::m_free = 0;
bool MemoryCounter::m_changed = true;

#ifdef ENABLEMEMORYCOUNTER
void* operator new(size_t size)
{
	MemoryCounter::AddAllocate(size);
	MemoryCounter::SetChanged(true);
	return malloc(size);
}

void operator delete(void* block, size_t size)
{
	MemoryCounter::AddFree(size);
	MemoryCounter::SetChanged(true);
	free(block);
}
#endif

MemoryCounter::~MemoryCounter()
{
}

void MemoryCounter::ShowStatsWindow()
{
#ifdef ENABLEMEMORYCOUNTER
	ImGui::Begin("MemoryCounter");

	if (m_changed)
	{
		m_changed = false;
		AddLog("Allocated:%ukb|Freed:%ukb|Dangling:%ukb", 
			m_allocate / 1000, 
			m_free / 1000, 
			(m_allocate - m_free) / 1000);
	}

	if (!m_buffer.empty())
		ImGui::TextUnformatted(&m_buffer[0], &m_buffer[m_buffer.size() - 1]);

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);
	ImGui::End();
#endif
}

void MemoryCounter::LogToFile(const char* filePath)
{
#ifdef ENABLEMEMORYCOUNTER
	std::ofstream file;
	file.open(filePath);
	if (!file.is_open())
		return;
	file << m_buffer.data();
	file.close();
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