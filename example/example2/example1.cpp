// example1.cpp: 定义应用程序的入口点。
//

#include "example1.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	//cv::namedWindow("Example1", cv::WINDOW_AUTOSIZE);

	/*
	1. Using the cv::RNG random number generator:
	a. Generate and print three floating-point numbers, each drawn from a uniform
	distribution from 0.0 to 1.0.
	b. Generate and print three double-precision numbers, each drawn from a
	Gaussian distribution centered at 0.0 and with a standard deviation of 1.0.
	c. Generate and print three unsigned bytes, each drawn from a uniform distri‐
	bution from 0 to 255.
	*/

	cv::RNG rng(cv::getTickCount());
	//for (int i = 0; i < 3; i++) {
	//	cout << rng.uniform(0.f, 1.f) << endl;
	//}

	//for (int i = 0; i < 3; i++) {
	//	cout << rng.gaussian(1.0) << endl;
	//}

	//cv::RNG rng2(256);
	//for (int i = 0; i < 3; i++) {
	//	cout << +(uchar)rng2 << endl;//notice + before uchar
	//}

	/*
	2. Using the fill() method of the cv::RNG random number generator, create an
	array of:
	a. 20 floating-point numbers with a uniform distribution from 0.0 to 1.0.
	b. 20 floating-point numbers with a Gaussian distribution centered at 0.0 and
	with a standard deviation of 1.0.
	c. 20 unsigned bytes with a uniform distribution from 0 to 255.
	d. 20 color triplets, each of three bytes with a uniform distribution from 0 to 255.
	*/

	cv::Mat mat(20, 1, CV_64FC1);
	//cout << mat << endl;
	cv::Mat a(1, 1, CV_64FC1);
	cv::Mat b(1, 1, CV_64FC1, cv::Scalar(1.f));
	rng.fill(mat, cv::RNG::UNIFORM, 0.f, 1.f);
	cout << mat << endl;
	rng.fill(mat, cv::RNG::NORMAL, 0.f, 1.f);
	cout << mat << endl;

	cv::Mat mat1(20, 1, CV_8UC1);
	//cout << mat1 << endl;
	cv::Mat a1(1, 1, CV_8UC1);
	cv::Mat b1(1, 1, CV_8UC1, cv::Scalar(256));
	rng.fill(mat1, cv::RNG::UNIFORM, 0, 256);
	cout << mat1 << endl;

	cv::Mat mat2(20, 1, CV_8UC3);
	//cout << "mat2: " << mat2 << endl;
	cv::Mat a2(3, 1, CV_8UC1);
	cv::Mat b2(3, 1, CV_8UC1, cv::Scalar(256));
	rng.fill(mat2, cv::RNG::UNIFORM, 0, 256);
	cout << mat2 << endl;


	/*3. Using the cv::RNG random number generator, create an array of 100 three-byte
objects such that:
a. The first and second dimensions have a Gaussian distribution, centered at 64
and 192, respectively, each with a variance of 10.
b. The third dimension has a Gaussian distribution, centered at 128 and with a
variance of 2.
c. Using the cv::PCA object, compute a projection for which maxComponents=2.
d. Compute the mean in both dimensions of the projection; explain the result.*/

	cv::Mat mat3(100, 1, CV_8UC3);
	//cout << "mat3 init: " << mat3 << endl;

	cv::Mat a3(3, 1, CV_8UC1, cv::Scalar(64, 192, 128));

	cv::Mat b3(3, 1, CV_8UC1, cv::Scalar(10, 10, 2));

	rng.fill(mat3, cv::RNG::NORMAL, a3, b3);

	cout << "mat3 after fill:" << mat3 << endl;
	cout << "mat3 col 0 channels " << mat3.col(0).channels() << endl;

	cv::Mat mat4 = cv::Mat::zeros(100, 3, CV_8UC1);
	cout << "mat4 init:" << mat4 << endl;
	mat4.data = mat3.data;
	
	cout << "mat4 copy from mat3:" << mat4 << endl;
	cout << "mat4 channels :" << mat4.channels() << endl;

	cv::Mat mean(1, 3, CV_8UC1, cv::Scalar(64, 192, 128));
	cv::PCA pca(mat4, cv::Mat(), cv::PCA::DATA_AS_ROW, 3);
	cv::Mat vec(100, 3, CV_8UC1, cv::Scalar(0, 0, 0));


	cout << "after pca project:"<< pca.project(mat4) << endl;



	//cv::waitKey(0);
	//cout << "Hello CMake." << endl;
	return 0;
}
