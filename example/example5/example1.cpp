// example1.cpp: 定义应用程序的入口点。
//

#include "example1.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

void help(const char **argv) {
    cout << "\n\n"
         << "This program solves the Exercises at the end of Chapter 11\n"
         << "Call:\n"
         << argv[0] << " <path/image_name>\n\n"
         << "For example: " << argv[0] << " /AverageMaleFace.jpg\n"
         << endl;
}

bool matIsEqual(const cv::Mat &mat1, const cv::Mat &mat2);

int main(int argc, char **argv) {

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <iamgename>\n" << endl;
        return -1;
    }
    /************************************************************************/
    /* 1.  Find  and  load  a  picture  of  a  face  where  the  face  is  frontal,  has  eyes  open,  and
    takes up most or all of the image area. Write code to find the pupils of the eyes.                                                                     */
    /************************************************************************/
    cv::Mat src = cv::imread(argv[1]);
    if (src.empty()) {
        cout << "can not load " << argv[1] << endl;
        return -1;
    }

    cv::Mat gray;
    cv::Mat tmp;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    //Laplacian pyramid
    cv::pyrDown(gray, tmp);
    cv::pyrUp(tmp, tmp);
    tmp = gray - tmp;
    double minPixelValue, maxPixelValue;
    cv::Point minPixelPoint, maxPixelPoint;
    // find and circle the result
    cv::minMaxLoc(tmp, &minPixelValue, &maxPixelValue, &minPixelPoint, &maxPixelPoint);
    cv::circle(src, maxPixelPoint, 10, cv::Scalar(255, 255, 255), 2);

    cv::namedWindow("example5", cv::WINDOW_AUTOSIZE);
    cv::imshow("example5", src);


    /**
     * 2. Look at the diagrams of how the log-polar function transforms a square into a
wavy line.
a. Draw the log-polar results if the log-polar center point were sitting on one of
the corners of the square.
b. What would a circle look like in a log-polar transform if the center point were
inside the circle and close to the edge?
c. Draw what the transform would look like if the center point were sitting just
outside of the circle.
     */

    cv::Mat blackBoard(512, 512, CV_8UC1);
    cv::Mat blackBoard2(512, 512, CV_8UC1);

    cv::Point leftTop(100, 100);
    cv::Point rightBottom(412, 412);

    cv::rectangle(blackBoard, leftTop, rightBottom, cv::Scalar(128), 4);

    cv::logPolar(blackBoard, blackBoard2, cv::Point2f(256.0f, 256.0f), 50.00, cv::INTER_LINEAR);

    cv::namedWindow("blackBoard", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("blackBoard2", cv::WINDOW_AUTOSIZE);

    cv::imshow("blackBoard", blackBoard);
    cv::imshow("blackBoard2", blackBoard2);

    cv::waitKey(0);
    //cout << "Hello CMake." << endl;
    return 0;
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
