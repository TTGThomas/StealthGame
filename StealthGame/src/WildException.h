#pragma once

#include <iostream>

class WildException : public std::exception
{
public:
	WildException(const char* text)
		: m_text(text) {}

	virtual const char* what() const override
	{
		return m_text;
	}
private:
	const char* m_text = nullptr;
};