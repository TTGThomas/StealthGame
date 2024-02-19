#pragma once

#include <iostream>
#include <vector>
#include <fstream>

#include <imgui.h>

class MemoryCounter
{
public:
	MemoryCounter() = default;
	~MemoryCounter();

	static void ShowStatsWindow();

	static void LogToFile(const char* filePath);

	static void AddLog(const char* text, ...);

	static void AddAllocate(size_t size) { m_allocate += size; }
	static void AddFree(size_t size) { m_free += size; }

	static void SetChanged(bool changed) { m_changed = changed; }
private:
	static std::vector<char> m_buffer;
	static size_t m_allocate;
	static size_t m_free;
	static bool m_changed;
};