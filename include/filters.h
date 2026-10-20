#ifndef FILTERS_H
#define FILTERS_H

#include <opencv2/opencv.hpp>

namespace rs {

// Median filter (removes salt-and-pepper noise)
cv::Mat medianFilter(const cv::Mat& src, int kernelSize = 5);

// Gaussian filter (smooths Gaussian noise)
cv::Mat gaussianFilter(const cv::Mat& src,
                       int kernelSize = 5,
                       double sigma   = 1.5);

// High-pass filter (edge/detail extraction)
// output = src - blurred
cv::Mat highPassFilter(const cv::Mat& src,
                       int kernelSize = 9,
                       double sigma   = 10.0);

// Unsharp masking (sharpening)
// output = src + amount * (src - blurred)
cv::Mat unsharpMask(const cv::Mat& src,
                    int kernelSize = 5,
                    double sigma   = 1.0,
                    double amount  = 1.5);

// Bilateral filter (edge-preserving smoothing)
cv::Mat bilateralFilter(const cv::Mat& src,
                        int d          = 9,
                        double sigmaC  = 75,
                        double sigmaS  = 75);

} // namespace rs

#endif // FILTERS_H