#ifndef NONLINEAR_H
#define NONLINEAR_H

#include <opencv2/opencv.hpp>

namespace rs {

// Gamma correction. gamma < 1 brightens, gamma > 1 darkens.
cv::Mat gammaCorrect(const cv::Mat& src, double gamma);

// Logarithmic transform: s = c * log(1 + r)
// c is auto-computed so max output = 255
cv::Mat logTransform(const cv::Mat& src);

// Inverse log (exponential) transform for brightening shadows
cv::Mat inverseLogTransform(const cv::Mat& src);

} // namespace rs

#endif // NONLINEAR_Hs