// example1.cpp: 定义应用程序的入口点。
//

#include "example1.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

bool matIsEqual(const cv::Mat mat1, const cv::Mat mat2);

int main(int argc, char** argv)
{

	/*
	1. Load an image with interesting textures. Smooth the image in several ways using
cv::smooth() with smoothtype=cv::GAUSSIAN.
a. Use a symmetric 3 × 3, 5 × 5, 9 × 9, and 11 × 11 smoothing window size and
display the results.
b. Are the output results nearly the same by smoothing the image twice with a
5 × 5 Gaussian filter as when you smooth once with two 11 × 11 filters? Why
or why not?
	*/

	cv::namedWindow("Example3", cv::WINDOW_AUTOSIZE);

	cv::Mat oriImage = cv::imread(argv[1]);

	cv::imshow("Example3", oriImage);

	cv::namedWindow("smooth 3x3", cv::WINDOW_AUTOSIZE);

	cv::Mat blur33(oriImage.size(), CV_16S, cv::Scalar(0, 0, 0));

	cv::blur(oriImage, blur33, cv::Size2d(3, 3));

	cv::imshow("smooth 3x3", blur33);

	cv::namedWindow("smooth 11x11", cv::WINDOW_AUTOSIZE);

	cv::Mat blur11x11(oriImage.size(), CV_16S, cv::Scalar(0, 0, 0));

	cv::blur(oriImage, blur11x11, cv::Size2d(11, 11));

	cv::imshow("smooth 11x11", blur11x11);

	cv::Mat blur55(oriImage.size(), CV_16S, cv::Scalar(0, 0, 0));
	cv::Mat blur55_55(oriImage.size(), CV_16S, cv::Scalar(0, 0, 0));
	cv::blur(oriImage, blur55, cv::Size2d(5, 5));
	cv::blur(blur55, blur55_55, cv::Size2d(5, 5));

	cout << "twice 5x5 and once 11x11 is same? " << matIsEqual(blur55_55, blur11x11) << endl;


	/*
	2. Create a 100 × 100 single-channel image. Set all pixels to 0. Finally, set the center
pixel equal to 255.
a. Smooth this image with a 5 × 5 Gaussian filter and display the results. What
did you find?
b. Do this again but with a 9 × 9 Gaussian filter.
c. What does it look like if you start over and smooth the image twice with the 5
× 5 filter? Compare this with the 9 × 9 results. Are they nearly the same? Why
or why not?

	*/

	cv::Mat mat100x100(100, 100, CV_8UC1, cv::Scalar(0));
	mat100x100.at<uchar>(50, 50) = 255;
	cv::Mat blur100x100(mat100x100.size(), CV_16SC1, cv::Scalar(0));
	cv::GaussianBlur(mat100x100, blur100x100, cv::Size(5, 5), 0, 0);

	cv::namedWindow("blur100x100", cv::WINDOW_AUTOSIZE);
	cv::imshow("blur100x100", blur100x100);

	cv::waitKey(0);
	//cout << "Hello CMake." << endl;
	return 0;
}

bool matIsEqual(const cv::Mat mat1, const cv::Mat mat2) {
	if (mat1.empty() && mat2.empty()) {
		return true;
	}
	if (mat1.cols != mat2.cols || mat1.rows != mat2.rows || mat1.dims != mat2.dims ||
		mat1.channels() != mat2.channels()) {
		return false;
	}
	if (mat1.size() != mat2.size() || mat1.channels() != mat2.channels() || mat1.type() != mat2.type()) {
		return false;
	}
	int nrOfElements1 = mat1.total() * mat1.elemSize();
	if (nrOfElements1 != mat2.total() * mat2.elemSize()) return false;
	bool lvRet = memcmp(mat1.data, mat2.data, nrOfElements1) == 0;
	return lvRet;
}
