// example1.cpp: 定义应用程序的入口点。
//

#include "example1.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

void help(char **argv) {
    cout << "\n\n"
         << "This program shows how to filter images with mask: the write\n "
         << "it yourself and the filter2d way.\n"
         << argv[0] << " <path/image_name>\n\n"
         << "For example: " << argv[0] << " /AverageMaleFace.jpg\n"
         << endl;
}

bool matIsEqual(const cv::Mat &mat1, const cv::Mat &mat2);

void sharpen(const cv::Mat &myImage, cv::Mat &Result);

int main(int argc, char **argv) {

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <iamgename>\n" << endl;
        return -1;
    }
    help(argv);

    cv::Mat src = cv::imread(argv[1]);

    cv::Mat dst0;
    auto t = (double) cv::getTickCount();
    sharpen(src, dst0);
    t = ((double) cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "Hand written function time passed in seconds: " << t << endl;
    cv::imshow("src", src);
    cv::imshow("dst0", dst0);

    cv::waitKey(0);

    cv::Mat kernel = (cv::Mat_<char>(3, 3) <<
                                           0, -1, 0,
                                           -1, 5, -1,
                                           0, -1, 0);

    cv::Mat dst1;
    t = (double) cv::getTickCount();
    cv::filter2D(src, dst1, src.depth(), kernel);
    t = ((double) cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "Built-in filter2D time passed in seconds:     " << t << endl;
    imshow("dst1", dst1);
    cv::waitKey(0);

//    cout << "Hello CMake." << endl;
    return 0;
}

void sharpen(const cv::Mat &myImage, cv::Mat &Result) {
    CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images
    const int nChannels = myImage.channels();
    Result.create(myImage.size(), myImage.type());
    for (int j = 1; j < myImage.rows - 1; ++j) {
        auto *previous = myImage.ptr<uchar>(j - 1);
        auto *current = myImage.ptr<uchar>(j);
        auto *next = myImage.ptr<uchar>(j + 1);
        auto *output = Result.ptr<uchar>(j);
        for (int i = nChannels; i < nChannels * (myImage.cols - 1); ++i) {
            *output++ = cv::saturate_cast<uchar>(5 * current[i]
                                                 - current[i - nChannels] - current[i + nChannels] - previous[i] -
                                                 next[i]);
        }
    }
    Result.row(0).setTo(cv::Scalar(0));
    Result.row(Result.rows - 1).setTo(cv::Scalar(0));
    Result.col(0).setTo(cv::Scalar(0));
    Result.col(Result.cols - 1).setTo(cv::Scalar(0));
}

bool matIsEqual(const cv::Mat &mat1, const cv::Mat &mat2) {
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
