#pragma once
#include <vector>


namespace asciify
{
	std::vector<int> generate_greyscale(int argc, char** argv);

	void generate_ascii(const std::vector<int>& intensities);

	void generate_video();
}