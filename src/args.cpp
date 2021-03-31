#include "../include/args.h"
#include "../include/asciify.h"
#include <fstream>
#include <sstream>
#include <Windows.h>


void args::parse_args(int argc, char** argv)
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
	else if (strcmp(argv[1], "image") == 0)
	{
		cmd_image(argc, argv);
		asciify::generate_ascii(asciify::generate_greyscale(argc, argv));
	}
	else
	{
		print_error("unrecognized command '" + std::string(argv[1]) + "'");
		exit(1);
	}
}


void args::help()
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	print_colored("multimedia to ascii art tool", 6);

	std::cout << "asciify image <image path> <args>: create ascii art from an image\n";
}


std::string args::next_arg(int argc, char** argv, int& i)
{
	if (++i >= argc)
	{
		print_error(std::string(argv[i - 1]) + ": missing required argument");

		exit(1);
	}

	return argv[i++];
}


void args::cmd_image(int argc, char** argv)
{
	if (argc >= 3)
		image::image_path = argv_get(argc, argv, 2);
	else
	{
		help_image();
		exit(1);
	}

	if (image::image_path == "--help")
	{
		help_image();
		exit(0);
	}

	{
		std::ifstream temp(image::image_path);

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
			image::write_to_file = true;
			image::output_path = next_arg(argc, argv, i);
		}

		else if (strcmp(argv[i], "-s") == 0)
		{
			std::stringstream(next_arg(argc, argv, i)) >> image::img_w;
			image::img_h = image::img_w;
		}

		else if (strcmp(argv[i], "-w") == 0)
			std::stringstream(next_arg(argc, argv, i)) >> image::img_w;

		else if (strcmp(argv[i], "-h") == 0)
			std::stringstream(next_arg(argc, argv, i)) >> image::img_h;

		else if (strcmp(argv[i], "-rs") == 0)
		{
			std::stringstream(next_arg(argc, argv, i)) >> image::resize_percent;

			if (image::resize_percent <= 0)
			{
				print_error("cant resize to 0% or less");
				exit(1);
			}

			image::resize_percent /= 100.f;
		}

		else if (strcmp(argv[i], "--open") == 0)
		{
			image::open = true;
			++i;
		}

		else if (strcmp(argv[i], "--help") == 0)
		{
			help_image();
			exit(0);
		}

		else
		{
			print_error("unrecognized argument '" + std::string(argv[i]) + "'");
			exit(1);
		}
	}
}


void args::help_image()
{
	print_colored("image help", 6);
	std::cout << "-f <filename>: write ascii output to a file\n";
	std::cout << "-s <size>: set image size to <size>x<size>\n";
	std::cout << "-w  <width>: set image width to <width>\n";
	std::cout << "-h <height>: set image height to <height>\n";
	std::cout << "-rs <resize percent>: resize image to percentage of its original size. -rs 75 resizes the image to 75%\n";
	std::cout << "--open: opens file automatically after writing output\n";
}


std::string args::argv_get(int argc, char** argv, int i)
{
	if (i >= argc)
	{
		print_error("expected argument for " + std::string(argv[i - 1]));
		exit(1);
	}

	return argv[i];
}


void args::print_error(const std::string& err)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(console, 4);
	std::cout << "error: " << err << "\n";
	SetConsoleTextAttribute(console, 7);
}


void args::print_colored(const std::string& text, DWORD color)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(console, color);
	std::cout << text << "\n";
	SetConsoleTextAttribute(console, 7);
}