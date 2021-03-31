#pragma once
#include <iostream>
#include <Windows.h>


namespace args
{
	namespace image
	{
		inline bool open = false;

		inline std::string image_path;

		inline bool write_to_file = false;
		inline std::string output_path;

		inline int img_w = 0;
		inline int img_h = 0;

		inline float resize_percent = 1.f;
	}


	void parse_args(int argc, char** argv);

	void help();
	std::string next_arg(int argc, char** argv, int& i);

	void cmd_image(int argc, char** argv);
	void help_image();

	std::string argv_get(int argc, char** argv, int i);

	void print_error(const std::string& err);
	void print_colored(const std::string& text, DWORD color);
}