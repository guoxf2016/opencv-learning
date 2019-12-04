// example1.cpp: 定义应用程序的入口点。
//

#include "example1.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

int main(int argc, char ** argv)
{
	cv::namedWindow("Example1", cv::WINDOW_AUTOSIZE);
	cv::VideoCapture cap;
	if (argc == 1) {
		cap.open(0);
	}
	else {
		cap.open(argv[1]);
	}

	if (!cap.isOpened()) {
		cerr << "Could not open capture!" << endl;
		return -1;
	}
	cv::Mat frame;
	for (;;) {
		cap >> frame;
		if (frame.empty()) {
			break;
		}
		cv::imshow("Example1", frame);
		if (cv::waitKey(33) >= 0) {
			break;
		}
	}
	//cout << "Hello CMake." << endl;
	return 0;
}
