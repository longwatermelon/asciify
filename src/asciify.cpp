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
		if (intensities[i] >= 210) image += '@';
		else if (intensities[i] >= 170) image += '#';
		else if (intensities[i] >= 150) image += '=';
		else if (intensities[i] >= 90) image += ':';
		else if (intensities[i] >= 70) image += '.';
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
	if (args::video::load_path != "")
	{
		return load_ascii_video();
	}

	std::vector<std::string> ascii_frames;

	cv::VideoCapture cap(args::video::video_path);

	if (!cap.isOpened())
	{
		utils::print_error("couldnt open video file '" + args::video::video_path + "'");
		exit(1);
	}

	if (args::video::fps == 0)
		args::video::fps = cap.get(cv::CAP_PROP_FPS);

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

	if (args::video::save_path != "")
	{
		save_ascii_video(ascii_frames);
	}

	cap.release();

	return ascii_frames;
}


std::vector<std::string> asciify::load_ascii_video()
{
	std::vector<std::string> ascii_frames;

	if (args::video::load_path != "")
	{
		std::ifstream f(args::video::load_path);

		if (f)
		{
			size_t read_frames = 0;

			std::stringstream ss;
			std::string line;

			std::getline(f, line);
			std::stringstream(line) >> args::video::fps;

			int accumulator = 0;

			while (std::getline(f, line))
			{
				ss << line;

				if (++accumulator >= args::video::video_h)
				{
					accumulator = 0;
					std::getline(f, line);

					ascii_frames.emplace_back(ss.str() + line);

					ss.clear();
					ss.str("");

					std::stringstream temp;
					temp << "\rread " << ++read_frames << " frames";
					std::cout << temp.str();
				}
			}

			std::cout << "\n";

			f.close();

			return ascii_frames;
		}
		else
		{
			utils::print_error("file '" + args::video::load_path + "' doesnt exist");
			exit(1);
		}
	}

	return ascii_frames;
}


void asciify::save_ascii_video(const std::vector<std::string>& ascii_frames)
{
	std::cout << "saving video...\n";

	{
		std::ofstream f(args::video::save_path, std::ofstream::out | std::ofstream::trunc);
		f.close();
	}

	std::fstream f(args::video::save_path);

	f << args::video::fps << "\n";

	for (int i = 0; i < ascii_frames.size(); ++i)
	{
		f << ascii_frames[i] << "\n";
		std::cout << utils::make_loading_bar((int)(0.75f * args::video::video_w), i + 1, ascii_frames.size());
	}

	f.close();

	std::cout << "\n";

	std::cout << "saved video to " << args::video::save_path << " successfully\n";
}


void asciify::play_video(std::vector<std::string>& frames)
{
	if (args::video::audio_path != "")
	{
		system(args::video::audio_path.c_str());
	}

	std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();

	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD bytes_written = 0;

	bool key_right = false;
	bool key_left = false;
	bool key_space = false;

	bool paused = false;

	std::chrono::high_resolution_clock::time_point tp0 = std::chrono::high_resolution_clock::now();

	for (int f = 0; f < frames.size(); ++f)
	{
		auto& frame = frames[f];

		for (int i = 0; i < frame.size(); ++i)
		{
			if (frame[i] == '\n')
			{
				frame.erase(frame.begin() + i--);
			}
		}

		WriteConsoleOutputCharacter(console, frame.c_str(), frame.size(), { 0, 0 }, &bytes_written);

		if (GetAsyncKeyState(VK_RIGHT) && !key_right)
		{
			f += args::video::fps * 5;
			key_right = true;
		}

		if (GetAsyncKeyState(VK_LEFT) && !key_left)
		{
			f -= args::video::fps * 5;
			key_left = true;
		}

		if (GetAsyncKeyState(VK_RIGHT) == 0) key_right = false;
		if (GetAsyncKeyState(VK_LEFT) == 0) key_left = false;

		if (GetAsyncKeyState(VK_SPACE) && !key_space)
		{
			paused = true;
			key_space = true;
		}

		while (paused)
		{
			if (GetAsyncKeyState(VK_SPACE) == 0) key_space = false;

			if (GetAsyncKeyState(VK_SPACE) && !key_space)
			{
				paused = false;
				key_space = true;
			}
		}

		if (GetAsyncKeyState(VK_SPACE) == 0) key_space = false;

		if (GetAsyncKeyState(VK_ESCAPE)) return;

		double frame_time = (1000.0 * 1000.0 / (double)(args::video::fps)) * (f + 1);

		tp2 = std::chrono::high_resolution_clock::now();
		
		auto delta = std::chrono::duration<double, std::micro>(tp2 - tp0);

		if (frame_time - delta.count() >= 0)
		{
			std::chrono::duration<double, std::micro> delta_us(frame_time - delta.count());
			auto delta_us_duration = std::chrono::duration_cast<std::chrono::microseconds>(delta_us);

			std::this_thread::sleep_for(std::chrono::microseconds(delta_us_duration.count()));
		}
	}
}