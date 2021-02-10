// example1.cpp: 定义应用程序的入口点。
//

#include "example1.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

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

void update_map(int &ind, Mat &map_x, Mat &map_y);

int main(int argc, char **argv) {

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <iamgename>\n" << endl;
        return -1;
    }

    cv::Mat src = cv::imread(argv[1]);
    if (src.empty()) {
        cout << "load pic failed" << endl;
        return -1;
    }

    Mat dst(src.size(), src.type());
    Mat map_x(src.size(), CV_32FC1);
    Mat map_y(src.size(), CV_32FC1);

    const char *remapWindow = "Remap demo";

    namedWindow(remapWindow, WINDOW_AUTOSIZE);

    int ind = 0;

    cout << "Enter 0 ~ 3" << endl;
    cin >> ind;

    update_map(ind, map_x, map_y);
    remap(src, dst, map_x, map_y, INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
    imshow(remapWindow, dst);

    imshow("Original Image", src);

    cv::waitKey(0);

//    cout << "Hello CMake." << endl;
    return 0;
}

void update_map(int &ind, Mat &map_x, Mat &map_y) {
    for (int i = 0; i < map_x.rows; i++) {
        for (int j = 0; j < map_x.cols; j++) {
            switch (ind) {
                case 0:
                    if (j > map_x.cols * 0.25 && j < map_x.cols * 0.75 && i > map_x.rows * 0.25 &&
                        i < map_x.rows * 0.75) {
                        map_x.at<float>(i, j) = 2 * (j - map_x.cols * 0.25f) + 0.5f;
                        map_y.at<float>(i, j) = 2 * (i - map_x.rows * 0.25f) + 0.5f;
                    } else {
                        map_x.at<float>(i, j) = 0;
                        map_y.at<float>(i, j) = 0;
                    }
                    break;
                case 1:
                    map_x.at<float>(i, j) = (float) j;
                    map_y.at<float>(i, j) = (float) (map_x.rows - i);
                    break;
                case 2:
                    map_x.at<float>(i, j) = (float) (map_x.cols - j);
                    map_y.at<float>(i, j) = (float) i;
                    break;
                case 3:
                    map_x.at<float>(i, j) = (float) (map_x.cols - j);
                    map_y.at<float>(i, j) = (float) (map_x.rows - i);
                    break;
                default:
                    break;
            }
        }
    }

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
