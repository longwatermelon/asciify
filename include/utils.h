#pragma once
#include <string>
#include <Windows.h>


namespace utils
{
	void print_error(const std::string& err);
	void print_colored(const std::string& text, DWORD color);
	void print_warning(const std::string& text);

	void get_screen_dimensions();

	std::string make_loading_bar(int width, size_t current_num, size_t final_num);
}