#include "../include/utils.h"
#include <iostream>


void utils::print_error(const std::string& err)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(console, 4);
	std::cout << "error: " << err << "\n";
	SetConsoleTextAttribute(console, 7);
}


void utils::print_colored(const std::string& text, DWORD color)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(console, color);
	std::cout << text << "\n";
	SetConsoleTextAttribute(console, 7);
}