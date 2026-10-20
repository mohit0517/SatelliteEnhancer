#include "filters.h"

namespace rs {

// ============================================================
//  Median Filter
// ============================================================
cv::Mat medianFilter(const cv::Mat& src, int kernelSize) {
    if (kernelSize % 2 == 0) kernelSize += 1;  // must be odd
    cv::Mat out;
    cv::medianBlur(src, out, kernelSize);
    return out;
}

// ============================================================
//  Gaussian Filter
// ============================================================
cv::Mat gaussianFilter(const cv::Mat& src, int kernelSize, double sigma) {
    if (kernelSize % 2 == 0) kernelSize += 1;
    cv::Mat out;
    cv::GaussianBlur(src, out, cv::Size(kernelSize, kernelSize), sigma);
    return out;
}

// ============================================================
//  High-Pass Filter
//  output = |src - blurred|, scaled to [0, 255]
// ============================================================
cv::Mat highPassFilter(const cv::Mat& src, int kernelSize, double sigma) {
    if (kernelSize % 2 == 0) kernelSize += 1;

    cv::Mat blurred, hp;
    cv::GaussianBlur(src, blurred, cv::Size(kernelSize, kernelSize), sigma);

    cv::Mat srcF, blurF, diff;
    src.convertTo(srcF, CV_32F);
    blurred.convertTo(blurF, CV_32F);
    diff = srcF - blurF;

    // Shift so values fit into 8-bit range
    diff += 128.0;
    cv::Mat out;
    diff.convertTo(out, CV_8U);
    return out;
}

// ============================================================
//  Unsharp Mask
//  output = src + amount * (src - blurred)
// ============================================================
cv::Mat unsharpMask(const cv::Mat& src, int kernelSize,
                    double sigma, double amount) {
    if (kernelSize % 2 == 0) kernelSize += 1;

    cv::Mat blurred;
    cv::GaussianBlur(src, blurred, cv::Size(kernelSize, kernelSize), sigma);

    cv::Mat srcF, blurF, outF;
    src.convertTo(srcF, CV_32F);
    blurred.convertTo(blurF, CV_32F);

    outF = srcF + amount * (srcF - blurF);

    cv::Mat out;
    outF.convertTo(out, CV_8U);
    return out;
}

// ============================================================
//  Bilateral Filter (edge-preserving)
// ============================================================
cv::Mat bilateralFilter(const cv::Mat& src, int d, double sigmaC, double sigmaS) {
    cv::Mat out;
    cv::bilateralFilter(src, out, d, sigmaC, sigmaS);
        return out;
}

} // namespace rs