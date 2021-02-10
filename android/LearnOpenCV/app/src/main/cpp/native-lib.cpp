#include <jni.h>
#include <string>

#include <android/log.h>
#include <android/bitmap.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>

#include <omp.h>

#define ASSERT(status, ret)     if (!(status)) { return ret; }
#define ASSERT_FALSE(status)    ASSERT(status, false)

#define  LOG_TAG    "LearnOpenCV"
#define  LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG, __VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, __VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)

bool BitmapToMatrix(JNIEnv *env, jobject obj_bitmap, cv::Mat &matrix) {
    void *bitmapPixels;                                            // 保存图片像素数据
    AndroidBitmapInfo bitmapInfo;                                   // 保存图片参数

    ASSERT_FALSE(AndroidBitmap_getInfo(env, obj_bitmap, &bitmapInfo) >= 0);        // 获取图片参数
    ASSERT_FALSE(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888
                 || bitmapInfo.format ==
                    ANDROID_BITMAP_FORMAT_RGB_565);          // 只支持 ARGB_8888 和 RGB_565
    ASSERT_FALSE(AndroidBitmap_lockPixels(env, obj_bitmap, &bitmapPixels) >= 0);  // 获取图片像素（锁定内存块）
    ASSERT_FALSE(bitmapPixels);

    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC4, bitmapPixels);    // 建立临时 mat
        tmp.copyTo(matrix);                                                         // 拷贝到目标 matrix
    } else {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC2, bitmapPixels);
        cv::cvtColor(tmp, matrix, cv::COLOR_BGR5652RGB);
    }

    AndroidBitmap_unlockPixels(env, obj_bitmap);            // 解锁
    return true;
}


bool MatrixToBitmap(JNIEnv *env, cv::Mat &matrix, jobject obj_bitmap) {
    void *bitmapPixels;                                            // 保存图片像素数据
    AndroidBitmapInfo bitmapInfo;                                   // 保存图片参数

    ASSERT_FALSE(AndroidBitmap_getInfo(env, obj_bitmap, &bitmapInfo) >= 0);        // 获取图片参数
    ASSERT_FALSE(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888
                 || bitmapInfo.format ==
                    ANDROID_BITMAP_FORMAT_RGB_565);          // 只支持 ARGB_8888 和 RGB_565
    ASSERT_FALSE(matrix.dims == 2
                 && bitmapInfo.height == (uint32_t) matrix.rows
                 && bitmapInfo.width == (uint32_t) matrix.cols);                   // 必须是 2 维矩阵，长宽一致
    ASSERT_FALSE(matrix.type() == CV_8UC1 || matrix.type() == CV_8UC3 || matrix.type() == CV_8UC4);
    ASSERT_FALSE(AndroidBitmap_lockPixels(env, obj_bitmap, &bitmapPixels) >= 0);  // 获取图片像素（锁定内存块）
    ASSERT_FALSE(bitmapPixels);

    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC4, bitmapPixels);
        switch (matrix.type()) {
            case CV_8UC1:
                cv::cvtColor(matrix, tmp, cv::COLOR_GRAY2RGBA);
                break;
            case CV_8UC3:
                cv::cvtColor(matrix, tmp, cv::COLOR_BGR2RGBA);
                break;
            case CV_8UC4:
                matrix.copyTo(tmp);
                break;
            default:
                AndroidBitmap_unlockPixels(env, obj_bitmap);
                return false;
        }
    } else {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC2, bitmapPixels);
        switch (matrix.type()) {
            case CV_8UC1:
                cv::cvtColor(matrix, tmp, cv::COLOR_GRAY2BGR565);
                break;
            case CV_8UC3:
                cv::cvtColor(matrix, tmp, cv::COLOR_RGB2BGR565);
                break;
            case CV_8UC4:
                cv::cvtColor(matrix, tmp, cv::COLOR_RGBA2BGR565);
                break;
            default:
                AndroidBitmap_unlockPixels(env, obj_bitmap);
                return false;
        }
    }
    AndroidBitmap_unlockPixels(env, obj_bitmap);                // 解锁
    return true;
}

jobject generateBitmap(JNIEnv *env, int width, int height) {

    jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls,
                                                            "createBitmap",
                                                            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jstring configName = env->NewStringUTF("ARGB_8888");
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(
            bitmapConfigClass, "valueOf",
            "(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/Enum;");

    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                       valueOfBitmapConfigFunction,
                                                       bitmapConfigClass, configName);

    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls,
                                                    createBitmapFunction,
                                                    width,
                                                    height, bitmapConfig);
//    env->DeleteLocalRef(bitmapCls);
//    env->DeleteLocalRef(bitmapConfigClass);
//    env->DeleteLocalRef(bitmapConfig);
//    env->DeleteLocalRef(configName);
// todo 局部引用在函数返回后会被GC回收,如果需要创建大量局部引用,则需要手动释放,避免JNI局部引用表的溢出
//todo 如果 Native 方法不会返回，那么自动释放局部引用就失效了，这时候就必须要手动释放
    return newBitmap;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_guoxf_learnopencv_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_guoxf_learnopencv_MainActivity_process(JNIEnv *env, jclass clazz,
                                                jstring image_file) {

    auto filePath = env->GetStringUTFChars(image_file, nullptr);


    cv::Mat imageMat = cv::imread(filePath, cv::IMREAD_COLOR);

    if (imageMat.empty()) {
        env->ReleaseStringUTFChars(image_file, filePath);
        return nullptr;
    }

    auto width = imageMat.cols;
    auto height = imageMat.rows;

    jobject bitmap = generateBitmap(env, width, height);

    auto result = MatrixToBitmap(env, imageMat, bitmap);

    if (!result) {
        bitmap = nullptr;
    }

    env->ReleaseStringUTFChars(image_file, filePath);

    return bitmap;

}

cv::Mat decodeByteBufferToMat(JNIEnv *env, jobject byte_buffer,
                              jint length) {
    auto *buffer = static_cast<unsigned char *>(env->GetDirectBufferAddress(byte_buffer));
    if (buffer == nullptr) {
        return cv::Mat();
    }
    std::vector<unsigned char> inputData(buffer, buffer + length);
    cv::Mat imageMat = cv::imdecode(inputData, cv::IMREAD_COLOR);
    return imageMat;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_guoxf_learnopencv_MainActivity_process2(JNIEnv *env, jclass clazz, jobject byte_buffer,
                                                 jint length) {

    cv::Mat imageMat = decodeByteBufferToMat(env, byte_buffer, length);

    if (imageMat.empty()) {
        return nullptr;
    }
    int divideWith = 128;
    uchar table[256];
    for (int i = 0; i < 256; ++i)
        table[i] = (uchar) (divideWith * (i / divideWith));
    cv::Mat lookUpTable(1, 256, CV_8U);
    uchar *p = lookUpTable.ptr();
    for (int i = 0; i < 256; ++i)
        p[i] = table[i];
    auto width = imageMat.cols;
    auto height = imageMat.rows;

    cv::LUT(imageMat, lookUpTable, imageMat);

    jobject bitmap = generateBitmap(env, width, height);

    auto result = MatrixToBitmap(env, imageMat, bitmap);

    if (!result) {
        bitmap = nullptr;
    }

    return bitmap;
}extern "C"
JNIEXPORT jobject JNICALL
Java_com_guoxf_learnopencv_MainActivity_process3(JNIEnv *env, jclass clazz, jobject byte_buffer,
                                                 jint length) {

    cv::Mat imageMat = decodeByteBufferToMat(env, byte_buffer, length);

    if (imageMat.empty()) {
        return nullptr;
    }

    cv::Mat kernel = (cv::Mat_<char>(3,3) <<
            0, -1,  0,
            -1,  5, -1,
            0, -1,  0);

    cv::filter2D(imageMat, imageMat, imageMat.depth(), kernel);

    auto width = imageMat.cols;
    auto height = imageMat.rows;
    jobject bitmap = generateBitmap(env, width, height);

    auto result = MatrixToBitmap(env, imageMat, bitmap);

    if (!result) {
        return nullptr;
    }
    return bitmap;
}extern "C"
JNIEXPORT jobject JNICALL
Java_com_guoxf_learnopencv_MainActivity_process4(JNIEnv *env, jclass clazz, jobject input_bitmap,
                                                 jobject input_bitmap2) {


    cv::Mat inputMat;
    if (!BitmapToMatrix(env, input_bitmap, inputMat) || inputMat.empty()) {
        LOGD("inputMat empty");
        return nullptr;
    }

    cv::Mat inputMat2;
    if (!BitmapToMatrix(env, input_bitmap2, inputMat2) || inputMat2.empty()) {
        LOGD("inputMat2 empty");
        return nullptr;
    }
    double alpha = 0.5;
    cv::Mat dst;
    cv::addWeighted(inputMat, alpha, inputMat2, (1 - alpha), 0.0, dst);

    auto width = dst.cols;
    auto height = dst.rows;
    jobject bitmap = generateBitmap(env, width, height);

    if (!MatrixToBitmap(env, dst, bitmap)) {
        return nullptr;
    }
    return bitmap;
}extern "C"
JNIEXPORT jobject JNICALL
Java_com_guoxf_learnopencv_MainActivity_process5(JNIEnv *env, jclass clazz, jobject byte_buffer,
                                                 jint length) {
    cv::Mat imageMat = decodeByteBufferToMat(env, byte_buffer, length);

    if (imageMat.empty()) {
        return nullptr;
    }

    cv::Mat newImage = cv::Mat::zeros(imageMat.size(), imageMat.type());
    imageMat.convertTo(newImage, -1, 2.2, 50);


    auto width = newImage.cols;
    auto height = newImage.rows;
    jobject bitmap = generateBitmap(env, width, height);

    if (!MatrixToBitmap(env, newImage, bitmap)) {
        return nullptr;
    }
    return bitmap;
}extern "C"
JNIEXPORT jobject JNICALL
Java_com_guoxf_learnopencv_MainActivity_process6(JNIEnv *env, jclass clazz, jobject byte_buffer,
                                                 jint length) {
    cv::Mat imageMat = decodeByteBufferToMat(env, byte_buffer, length);

    if (imageMat.empty()) {
        return nullptr;
    }
    cv::Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    double gamma_ = 0.4;
    for( int i = 0; i < 256; ++i)
        p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma_) * 255.0);
    cv::Mat res = imageMat.clone();
    LUT(imageMat, lookUpTable, res);

    auto width = res.cols;
    auto height = res.rows;
    jobject bitmap = generateBitmap(env, width, height);

    if (!MatrixToBitmap(env, res, bitmap)) {
        return nullptr;
    }
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_guoxf_learnopencv_MainActivity_process7(JNIEnv *env, jclass clazz, jobject byte_buffer,
                                                 jint length) {
    cv::Mat imageMat = decodeByteBufferToMat(env, byte_buffer, length);

    if (imageMat.empty()) {
        return nullptr;
    }

    cv::Mat newImage = cv::Mat::zeros(imageMat.size(), CV_8UC1);
    cv::cvtColor(imageMat, newImage, cv::COLOR_BGR2GRAY);

    auto width = newImage.cols;
    auto height = newImage.rows;
    jobject bitmap = generateBitmap(env, width, height);

    if (!MatrixToBitmap(env, newImage, bitmap)) {
        return nullptr;
    }
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_guoxf_learnopencv_MainActivity_process8(JNIEnv *env, jclass clazz, jobject byte_buffer,
                                                 jint length, jobject lut3d) {
    cv::Mat imageMat = decodeByteBufferToMat(env, byte_buffer, length);

    if (imageMat.empty()) {
        return nullptr;
    }

    cv::Mat lut3dMat;
    if (!BitmapToMatrix(env, lut3d, lut3dMat) || lut3dMat.empty()) {
        LOGD("inputMat empty");
        return nullptr;
    }

    int channels = imageMat.channels();
    int nRows = imageMat.rows;
    int nCols = imageMat.cols * channels;
    if (imageMat.isContinuous())
    {
        nCols *= nRows;
        nRows = 1;
    }
    LOGD("channels %d, rows %d, cols %d", channels, nRows, nCols);
    LOGD("channels %d, rows %d, cols %d", lut3dMat.channels(), lut3dMat.rows, lut3dMat.cols);
    int i,j;
    uchar* p;

    for( i = 0; i < nRows; ++i)
    {
        p = imageMat.ptr(i);
#pragma omp parallel for num_threads(5) schedule(static, 1000)
        for ( j = 0; j < nCols; j+=channels)
        {

            LOGD("ori color %d, %d, %d", p[j], p[j +1], p[j+2]);
            auto b = p[j] / 4;
            auto g = p[j + 1] / 4;
            auto r = p[j + 2] / 4;
            LOGD("ori color divide 4  %d, %d, %d", b, g, r);

            auto stepRow = lut3dMat.rows / 8;
            auto stepCol = lut3dMat.cols / 8;
            LOGD("step %d, %d", stepRow, stepCol);
            auto _row = b / 8;
            auto _col = b % 8;

            LOGD("b index  %d, %d", _row, _col);
            auto indexRow = _row * stepRow + g;
            auto indexCol = _col * stepCol + r;
            LOGD("final index  %d, %d", indexRow, indexCol);
            auto color = lut3dMat.at<cv::Vec4b>(indexRow, indexCol);
            p[j] = color[2];//color rgb, target bgr
            p[j + 1] = color[1];
            p[j + 2] = color[0];
            LOGD("final color  %d, %d, %d", p[j], p[j +1], p[j+2]);
        }
    }

    jobject bitmap = generateBitmap(env, imageMat.cols, imageMat.rows);

    if (!MatrixToBitmap(env, imageMat, bitmap)) {
        return nullptr;
    }

    return bitmap;

}