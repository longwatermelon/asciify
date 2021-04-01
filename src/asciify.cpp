#include "../include/asciify.h"
#include "../include/args.h"
#include "../include/utils.h"
#include <string>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>


std::vector<int> asciify::generate_greyscale(int argc, char** argv, const cv::Mat& imgtemp)
{
	cv::Mat img;

	if (args::image::active)
	{
		if (args::image::img_w == 0)
			args::image::img_w = (int)(imgtemp.cols * args::image::resize_percent);
		if (args::image::img_h == 0)
			args::image::img_h = (int)(imgtemp.rows * args::image::resize_percent);

		args::image::img_h /= 2;

		cv::resize(imgtemp, img, cv::Size(args::image::img_w, args::image::img_h));
	}
	else
	{
		cv::resize(imgtemp, img, cv::Size(args::video::video_w, args::video::video_h));
	}

	std::vector<int> intensities;

	for (int i = 0; i < img.rows; ++i)
	{
		for (int j = 0; j < img.cols; ++j)
		{
			cv::Vec3b intensity = img.at<cv::Vec3b>(i, j);
			std::array<int, 3> rgb;

			for (int k = 0; k < img.channels(); ++k)
			{
				rgb[k] = intensity.val[k];
			}

			int in = (int)(0.2126f * (float)rgb[2] + 0.7152f * (float)rgb[1] + 0.0722f * (float)rgb[0]);

			intensities.emplace_back(in);
		}
	}

	return intensities;
}


std::string asciify::generate_ascii(const std::vector<int>& intensities, int& img_w)
{
	std::string image;

	for (int i = 0; i < intensities.size(); ++i)
	{
		if (intensities[i] >= 200) image += '@';
		else if (intensities[i] >= 150) image += '#';
		else if (intensities[i] >= 100) image += '=';
		else if (intensities[i] >= 65) image += ':';
		else if (intensities[i] >= 45) image += '.';
		else image += ' ';

		if (i % img_w == 0)
		{
			image += "\n";
		}
	}

	return image;
}


std::vector<std::string> asciify::generate_video(int argc, char** argv)
{
	std::vector<std::string> ascii_frames;

	cv::VideoCapture cap(args::video::video_path);

	if (!cap.isOpened())
	{
		utils::print_error("couldnt open video file '" + args::video::video_path + "'");
		exit(1);
	}

	if (args::video::fps == 0)
		args::video::fps = cap.get(cv::CAP_PROP_FPS);

	if (args::video::video_w == 0)
		args::video::video_w = 400;
	if (args::video::video_h == 0)
		args::video::video_h = 200;

	for (int i = 0; i < cap.get(cv::CAP_PROP_FRAME_COUNT); ++i)
	{
		cap.set(cv::CAP_PROP_POS_FRAMES, i);
		cv::Mat frame;

		if (!cap.read(frame) || !frame.data)
		{
			std::cout << "\n";
			utils::print_error("couldnt read frame " + std::to_string(i));
			break;
		}

		ascii_frames.emplace_back(generate_ascii(generate_greyscale(argc, argv, frame), args::video::video_w));

		std::cout << utils::make_loading_bar((int)(0.75f * args::video::video_w), i + 1, cap.get(cv::CAP_PROP_FRAME_COUNT));

		frame.release();
	}

	std::cout << "\nfinished generating frames\n";

	cap.release();

	return ascii_frames;
}


void asciify::play_video(const std::vector<std::string>& frames)
{
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD bytes_written = 0;

	for (auto& frame : frames)
	{
		std::string screen;

		for (int i = 2; i < frame.size(); ++i)
		{
			if (frame[i] == '\n')
			{
				while (i % args::video::video_w != 0 && frame[i] != '\n')
				{
					++i;
					screen += ' ';
				}
			}
			else
			{
				screen += frame[i];
			}
		}

		WriteConsoleOutputCharacter(console, screen.c_str(), screen.size(), { 0, 0 }, &bytes_written);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / args::video::fps));
	}
}