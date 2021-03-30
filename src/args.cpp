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
		print_error("unrecognized command '" + std::string(argv[1]) + "'");
		exit(1);
	}
}


void help()
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	print_colored("image to ascii art tool", 14);

	std::cout << "asciify make <image path> <args>: create ascii art from an image\n";
}


std::string next_arg(int argc, char** argv, int& i)
{
	if (++i >= argc)
	{
		print_error(std::string(argv[i - 1]) + ": missing required argument");

		exit(1);
	}

	return argv[i++];
}


void cmd_make(int argc, char** argv)
{
	if (argc >= 3)
		args::path = argv_get(argc, argv, 2);
	else
	{
		help_make();
		exit(1);
	}

	if (args::path == "--help")
	{
		help_make();
		exit(0);
	}

	{
		std::ifstream temp(args::path);

		if (!temp)
		{
			print_error("file doesnt exist");
			exit(1);
		}

		temp.close();
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
				print_error("cant resize to 0% or less");
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
			print_error("unrecognized argument '" + std::string(argv[i]) + "'");
			exit(1);
		}
	}
}


void help_make()
{
	print_colored("make help", 14);
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
		print_error("expected argument for " + std::string(argv[i - 1]));
		exit(1);
	}

	return argv[i];
}


void print_error(const std::string& err)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(console, 4);
	std::cout << "error: " << err << "\n";
	SetConsoleTextAttribute(console, 7);
}


void print_colored(const std::string& text, DWORD color)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(console, color);
	std::cout << text << "\n";
	SetConsoleTextAttribute(console, 7);
}