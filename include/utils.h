#pragma once
#include <string>
#include <Windows.h>


namespace utils
{
	void print_error(const std::string& err);
	void print_colored(const std::string& text, DWORD color);
}