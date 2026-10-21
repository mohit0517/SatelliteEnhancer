#include "edge_enhance.h"

namespace rs {

cv::Mat robertsEdge(const cv::Mat& src) {
    cv::Mat gray, gx, gy, dst;
    if (src.channels() == 3) cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    else gray = src;

    cv::Mat kx = (cv::Mat_<float>(2, 2) << 1, 0, 0, -1);
    cv::Mat ky = (cv::Mat_<float>(2, 2) << 0, 1, -1, 0);

    cv::filter2D(gray, gx, CV_32F, kx);
    cv::filter2D(gray, gy, CV_32F, ky);
    cv::magnitude(gx, gy, dst);
    dst.convertTo(dst, CV_8U);
    return dst;
}

cv::Mat sobelEdge(const cv::Mat& src) {
    cv::Mat gray, gx, gy, dst;
    if (src.channels() == 3) cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    else gray = src;

    cv::Sobel(gray, gx, CV_32F, 1, 0, 3);
    cv::Sobel(gray, gy, CV_32F, 0, 1, 3);
    cv::magnitude(gx, gy, dst);
    dst.convertTo(dst, CV_8U);
    return dst;
}

cv::Mat laplacianEdge(const cv::Mat& src) {
    cv::Mat gray, dst;
    if (src.channels() == 3) cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    else gray = src;

    cv::Laplacian(gray, dst, CV_32F, 3);
    cv::convertScaleAbs(dst, dst);
    return dst;
}

cv::Mat laplacianSharpen(const cv::Mat& src) {
    cv::Mat lap, sharpened;

    // Calculate Laplacian on the original image (keeps color if src is color)
    cv::Laplacian(src, lap, CV_32F, 3);

    // Convert Lap to 8-bit so it matches the original image type
    cv::Mat lap8u;
    cv::convertScaleAbs(lap, lap8u);

    // Add the edges back to the original image
    cv::addWeighted(src, 1.0, lap8u, 1.0, 0, sharpened);

    return sharpened;
}

} // namespace rs