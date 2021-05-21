#include "utils.h"
#include "args.h"
#include <iostream>
#include <sstream>
#include <fstream>


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


void utils::print_warning(const std::string& text)
{
	print_colored(text, 6);
}

void utils::get_screen_dimensions()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int cols, rows;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	args::video::video_w = cols;
	args::video::video_h = rows;
}


std::string utils::make_loading_bar(int width, size_t current_num, size_t final_num)
{
	std::string bar = "\r[";

	for (int i = 1; i < width; ++i)
	{
		if ((float)((float)i / (float)width) <= (float)((float)current_num / (float)final_num))
		{
			bar += '#';
		}
		else
		{
			bar += '-';
		}
	}

	bar += ']';

	std::stringstream ss;
	ss << current_num << " / " << final_num << "  (" << (float)(current_num * 100 / final_num) << "%)";
	bar += " " + ss.str();

	return bar;
}


void utils::log(const std::string& fp, const std::string& text)
{
	std::ofstream of(fp, std::ios_base::app);
	of << text << "\n";
	of.close();
}