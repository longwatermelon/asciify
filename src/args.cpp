#include "../include/args.h"
#include <fstream>
#include <sstream>
#include <Windows.h>


void parse_args(int argc, char** argv)
{
	if (argc == 1)
	{
		help();
		exit(0);
	}

	if (strcmp(argv[1], "--help") == 0)
	{
		help();
		exit(0);
	}
	else if (strcmp(argv[1], "make") == 0)
	{
		cmd_make(argc, argv);
	}
	else
	{
		std::cout << "unrecognized command: " << argv[1] << "\n";
		exit(1);
	}
}


void help()
{
	std::cout << "image to ascii art tool\n\n";
	std::cout << "asciify make <image path> <args>: create ascii art from an image\n";
}


std::string next_arg(int argc, char** argv, int& i)
{
	if (++i >= argc)
	{
		std::cout << argv[i - 1] << ": missing required argument\n";
		exit(1);
	}

	return argv[i++];
}


void cmd_make(int argc, char** argv)
{
	if (argc >= 3)
		args::path = argv_get(argc, argv, 2);

	if (args::path == "--help")
	{
		help_make();
		exit(0);
	}

	int i = 3;
	while (i < argc)
	{
		if (strcmp(argv[i], "-f") == 0)
		{
			args::write_to_file = true;
			args::filename = next_arg(argc, argv, i);
		}

		else if (strcmp(argv[i], "-s") == 0)
		{
			std::stringstream(next_arg(argc, argv, i)) >> args::img_w;
			args::img_h = args::img_w;
		}

		else if (strcmp(argv[i], "-w") == 0)
			std::stringstream(next_arg(argc, argv, i)) >> args::img_w;

		else if (strcmp(argv[i], "-h") == 0)
			std::stringstream(next_arg(argc, argv, i)) >> args::img_h;

		else if (strcmp(argv[i], "-rs") == 0)
		{
			std::stringstream(next_arg(argc, argv, i)) >> args::resize_percent;

			if (args::resize_percent <= 0)
			{
				std::cout << "can't resize to 0% or less\n";
				exit(1);
			}

			args::resize_percent /= 100.f;
		}

		else if (strcmp(argv[i], "--open") == 0)
		{
			args::open = true;
			++i;
		}

		else if (strcmp(argv[i], "--help") == 0)
		{
			help_make();
			exit(0);
		}

		else
		{
			std::cout << "unrecognized argument: " << argv[i] << "\n";
			exit(1);
		}
	}
}


void help_make()
{
	std::cout << "make help\n\n";
	std::cout << "-f <filename>: write ascii output to a file\n";
	std::cout << "-s <size>: set image size to <size>x<size>\n";
	std::cout << "-w  <width>: set image width to <width>\n";
	std::cout << "-h <height>: set image height to <height>\n";
	std::cout << "-rs <resize percent>: resize image to percentage of its original size. -rs 75 resizes the image to 75%\n";
	std::cout << "--open: opens file automatically after writing output\n";
}


std::string argv_get(int argc, char** argv, int i)
{
	if (i >= argc)
	{
		std::cout << "expected argument for: " << argv[i - 1] << "\n";
		exit(1);
	}

	return argv[i];
}