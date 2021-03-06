#include "args.h"
#include "asciify.h"
#include "utils.h"
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
		
		std::cout << asciify::generate_ascii(asciify::generate_greyscale(argc, argv, img), args::image::img_w) << "\n";
	}

	else if (strcmp(argv[1], "video") == 0)
	{
		video::active = true;

		utils::get_screen_dimensions();

		cmd_video(argc, argv);

		utils::print_warning("warning: ascii video size will be the same as the current terminal size (" + std::to_string(args::video::video_w) + " x " + std::to_string(args::video::video_h) + ")");

		std::vector<std::string> frames = asciify::generate_video(argc, argv);

		std::cout << "=== video controls ===\n";
		std::cout << "ESC to exit the video\n";
		std::cout << "space to toggle between pause and resume\n";
		std::cout << "left and right arrow keys to fast forward and backwards through the video\n";

		std::cout << "play video? (y/n) ";
		{
			std::string resp;
			std::getline(std::cin, resp);

			if (resp != "y")
			{
				exit(1);
			}
		}

		asciify::play_video(frames);

		std::cout << "finished playing video\n";
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
	std::cout << "asciify video <video path> <args>: asciify a video\n";
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
		exit(0);
	}

	if (image::image_path == "--help")
	{
		help_image();
		exit(0);
	}

	int i = 3;

	{
		std::ifstream temp(image::image_path);

		if (!temp)
		{
			utils::print_error("file doesnt exist");
			exit(1);
		}

		temp.close();
	}

	while (i < argc)
	{
		if (strcmp(argv[i], "-s") == 0)
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
	std::cout << "-s <size>: set image size to <size>x<size>\n";
	std::cout << "-w  <width>: set image width to <width>\n";
	std::cout << "-h <height>: set image height to <height>\n";
	std::cout << "-rs <resize percent>: resize image to percentage of its original size. -rs 75 resizes the image to 75%\n";
	std::cout << "--open: opens file automatically after writing output\n";
}


void args::cmd_video(int argc, char** argv)
{
	if (argc >= 3)
		video::video_path = argv_get(argc, argv, 2);
	else
	{
		help_video();
		exit(0);
	}

	if (video::video_path == "--help")
	{
		help_video();
		exit(0);
	}

	if (video::video_path.substr(video::video_path.size() - 4, 4) == ".txt")
	{
		std::cout << "loading existing ascii video file\n";
		video::load_path = video::video_path;
	}

	{
		std::ifstream temp(video::video_path);

		if (!temp)
		{
			utils::print_error("file '" + video::video_path + "' doesnt exist");
			exit(1);
		}

		temp.close();
	}

	int i = 3;
	while (i < argc)
	{
		if (strcmp(argv[i], "--help") == 0)
		{
			help_video();
			exit(0);
		}
		else if (strcmp(argv[i], "-f") == 0)
		{
			std::stringstream(next_arg(argc, argv, i)) >> video::fps;
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			video::save_path = next_arg(argc, argv, i);
		}
		else if (strcmp(argv[i], "-a") == 0)
		{
			video::audio_path = next_arg(argc, argv, i);
		}
		else
		{
			utils::print_error("unrecognized argument '" + std::string(argv[i]) + "'");
			exit(1);
		}
	}
}


void args::help_video()
{
	utils::print_colored("video help", 6);
	std::cout << "asciify video <video path> <args>\n";
	std::cout << "if video path ends in .txt, asciify will automatically load a previously saved ascii video to increase loading speed\n";
	std::cout << "-f <fps>: set fps of output video, default set to original video fps\n";
	std::cout << "-s <save path>: saves ascii video to a file, must be a .txt file or else asciify wont automatically pick up on an existing ascii file\n";
	std::cout << "-a <audio path>: path to audio for the video\n";
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