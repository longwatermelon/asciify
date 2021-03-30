#pragma once
#include <iostream>
#include <Windows.h>


namespace args
{
	inline HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	inline bool open = false;

	inline std::string path;

	inline bool write_to_file = false;
	inline std::string filename;

	inline int img_w = 0;
	inline int img_h = 0;

	inline float resize_percent = 1.f;
}

void parse_args(int argc, char** argv);

void help();
std::string next_arg(int argc, char** argv, int& i);

void cmd_make(int argc, char** argv);
void help_make();

std::string argv_get(int argc, char** argv, int i);