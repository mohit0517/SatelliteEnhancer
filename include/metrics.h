#ifndef METRICS_H
#define METRICS_H

#include <opencv2/opencv.hpp>

namespace rs {

// Calculates Mean Squared Error between two images
double computeMSE(const cv::Mat& img1, const cv::Mat& img2);

// Calculates Peak Signal-to-Noise Ratio (in dB)
double computePSNR(const cv::Mat& img1, const cv::Mat& img2);

// Calculates Structural Similarity Index (SSIM)
double computeSSIM(const cv::Mat& img1, const cv::Mat& img2);

} // namespace rs

#endif // METRICS_H