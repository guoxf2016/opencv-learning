// example1.cpp: 定义应用程序的入口点。
//

#include "example1.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

bool matIsEqual(const cv::Mat mat1, const cv::Mat mat2);

int main(int argc, char** argv)
{

	if (argc != 2) {
		cout << "Usage: " << argv[0] << " <iamgename>\n" << endl;
		return -1;
	}

	cv::Mat src = cv::imread(argv[1]);
	if (src.empty()) {
		cout << "can not load " << argv[1] <<endl;
		return -1;
	}

	cv::Point2f srcTri[] = {
		cv::Point2f(0, 0 ),
		cv::Point2f(src.cols - 1, 0),
		cv::Point2f(0, src.rows - 1)
	};

	cv::Point2f dstTri[] = {
		cv::Point2f(src.cols * 0.f, src.rows * 0.33f), // dst Top left
		cv::Point2f(src.cols * 0.85f, src.rows * 0.25f), // dst Top right
		cv::Point2f(src.cols * 0.15f, src.rows * 0.7f) // dst Bottom left
	};

	cv::Mat warp_mat = cv::getAffineTransform(srcTri, dstTri);

	cv::Mat dst, dst2;

	cv::warpAffine(src, dst, warp_mat, src.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT);

	for (int i = 0; i < 3; ++i)
		cv::circle(dst, dstTri[i], 5, cv::Scalar(255, 0, 255), -1);
	cv::imshow("Affine Transform Test", dst);

	cv::waitKey(0);

	for (int frame = 0; ; ++frame) {
		cv::Point2f center(src.cols * 0.5f, src.rows * 0.5f);
		double angle = frame * 3 % 360, scale = (cos((angle - 60) * CV_PI / 180) + 1.05) * 0.8;
		cv::Mat rot_mat = cv::getRotationMatrix2D(center, angle, scale);
		cv::warpAffine(
			src,
			dst,
			rot_mat,
			src.size(),
			cv::INTER_LINEAR,
			cv::BORDER_CONSTANT,
			cv::Scalar()
		);
		cv::imshow("Rotated Image", dst);
		if (cv::waitKey(30) >= 0)
			break;

	}

	cv::Point2f srcQuad[] = {
		cv::Point2f(0, 0), // src Top left
		cv::Point2f(src.cols - 1, 0), // src Top right
		cv::Point2f(src.cols - 1, src.rows - 1), // src Bottom right
		cv::Point2f(0, src.rows - 1) // src Bottom left
	};

	cv::Point2f dstQuad[] = {
		cv::Point2f(src.cols * 0.05f, src.rows * 0.33f),
		cv::Point2f(src.cols * 0.9f, src.rows * 0.25f),
		cv::Point2f(src.cols * 0.8f, src.rows * 0.9f),
		cv::Point2f(src.cols * 0.2f, src.rows * 0.7f)
	};

	cv::Mat warp_mat2 = cv::getPerspectiveTransform(srcQuad, dstQuad);

	cv::warpPerspective(src, dst2, warp_mat2, src.size(), cv::INTER_LINEAR,
		cv::BORDER_CONSTANT, cv::Scalar());

	for (int i = 0; i < 4; i++)
		cv::circle(dst2, dstQuad[i], 5, cv::Scalar(255, 0, 255), -1);
	cv::imshow("Perspective Transform Test", dst2);

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
