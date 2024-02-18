#pragma once

#include <iostream>
#include <vector>

#include <imgui.h>

class MemoryCounter
{
public:
	static void ShowStatsWindow();

	static void AddLog(const char* text, ...);

	static void AddAllocate(size_t size) { m_allocate += size; }
	static void AddFree(size_t size) { m_free += size; }
private:
	static std::vector<char> m_buffer;
	static size_t m_allocate;
	static size_t m_free;
};