#include "../include/args.h"
#include "../include/asciify.h"
#include "../include/utils.h"
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>


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
		image::active = true;
		cmd_image(argc, argv);

		cv::Mat img = cv::imread(image::image_path);
		
		if (args::image::write_to_file)
		{
			std::fstream f(args::image::output_path, std::ofstream::out | std::ofstream::trunc);
			f << asciify::generate_ascii(asciify::generate_greyscale(argc, argv, img), args::image::img_w);
			f.close();

			std::cout << "wrote output to " << args::image::output_path << "\n";

			if (args::image::open)
			{
				system(args::image::output_path.c_str());
			}
		}
		else
		{
			std::cout << asciify::generate_ascii(asciify::generate_greyscale(argc, argv, img), args::image::img_w) << "\n";

			if (args::image::img_w >= 120 || args::image::img_h >= 200)
			{
				utils::print_colored("ascii output is very large, run asciify image --help for more information", 6);
			}
		}
	}

	else
	{
		utils::print_error("unrecognized command '" + std::string(argv[1]) + "'");
		exit(1);
	}
}


void args::help()
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	utils::print_colored("multimedia to ascii art tool", 6);

	std::cout << "asciify image <image path> <args>: create ascii art from an image\n";
}


std::string args::next_arg(int argc, char** argv, int& i)
{
	if (++i >= argc)
	{
		utils::print_error(std::string(argv[i - 1]) + ": missing required argument");

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
			utils::print_error("file doesnt exist");
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
				utils::print_error("cant resize to 0% or less");
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
			utils::print_error("unrecognized argument '" + std::string(argv[i]) + "'");
			exit(1);
		}
	}
}


void args::help_image()
{
	utils::print_colored("image help", 6);
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
		utils::print_error("expected argument for " + std::string(argv[i - 1]));
		exit(1);
	}

	return argv[i];
}