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

    /**
     *  Using cv::dft() and cv::idft() to accelerate the computation of
     *  convolutions
     */

    cv::Mat A = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    if (A.empty()) {
        cout << "Cannot load " << argv[1] << endl;
        return -1;
    }
    cout << "Image size " << A.size << endl;
//    cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Image", A);

    cv::Size patchSize( 100, 100 );
    cv::Point topleft( A.cols/2, A.rows/2 );
    cv::Rect roi( topleft.x, topleft.y, patchSize.width, patchSize.height );
    cv::Mat B = A( roi );

    cv::imshow("ImageB", B);

    int dft_M = cv::getOptimalDFTSize( A.rows+B.rows-1 );
    int dft_N = cv::getOptimalDFTSize( A.cols+B.cols-1 );
    cv::Mat dft_A = cv::Mat::zeros( dft_M, dft_N, CV_32F );
    cv::Mat dft_B = cv::Mat::zeros( dft_M, dft_N, CV_32F );
    cv::Mat dft_A_part = dft_A( cv::Rect(0, 0, A.cols,A.rows) );
    cv::Mat dft_B_part = dft_B( cv::Rect(0, 0, B.cols,B.rows) );
    A.convertTo( dft_A_part, dft_A_part.type(), 1, -mean(A)[0] );
    B.convertTo( dft_B_part, dft_B_part.type(), 1, -mean(B)[0] );
    cv::dft( dft_A, dft_A, 0, A.rows );
    cv::dft( dft_B, dft_B, 0, B.rows );
    // set the last parameter to false to compute convolution instead of correlation
    //
    cv::mulSpectrums( dft_A, dft_B, dft_A, 0, true );
    cv::idft( dft_A, dft_A, cv::DFT_SCALE, A.rows + B.rows - 1 );
    cv::Mat corr = dft_A( cv::Rect(0, 0, A.cols + B.cols - 1, A.rows + B.rows - 1) );
    cv::normalize( corr, corr, 0, 1, cv::NORM_MINMAX, corr.type() );
    cv::pow( corr, 3., corr );
    B ^= cv::Scalar::all( 255 );
    cv::imshow( "Correlation", corr );

    cv::waitKey(0);
//    cout << "Hello CMake." << endl;
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
