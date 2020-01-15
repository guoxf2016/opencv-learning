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

    Mat gaussianBlur;
    GaussianBlur(src, gaussianBlur, Size(3, 3), 0, 0);

    Mat gray;
    cvtColor(gaussianBlur, gray, COLOR_BGR2GRAY);


    Canny(gray, gray, 50, 200, 3);

    Mat color;
    cvtColor(gray, color, COLOR_GRAY2BGR);
    Mat colorP = color.clone();

    vector<Vec2f> lines;
    HoughLines(gray, lines, 1, CV_PI / 180, 80, 0, 0);

    cout << "HoughLines " << lines.size() << endl;
    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0];
        float theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x_0 = rho * a, y_0 = rho * b;
        pt1.x = cvRound((double)rho / (a + 0.001));
        pt1.y = 0;
        pt2.x = 0;
        pt2.y = cvRound((double)rho / (b + 0.001));
        line( color, pt1, pt2, Scalar(0,255,0), 3, LINE_AA);
    }


    vector<Vec4i> linesP; // will hold the results of the detection
    HoughLinesP(gray, linesP, 1, CV_PI/180, 50, 50, 10 ); // runs the actual detection
    // Draw the lines
    for( size_t i = 0; i < linesP.size(); i++ )
    {
        Vec4i l = linesP[i];
        line( colorP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
    }
    imshow("src", src);
    imshow("hough", color);
    imshow("houghP", colorP);
    //imshow("Original Image", src);

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
