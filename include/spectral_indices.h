#ifndef SPECTRAL_INDICES_H
#define SPECTRAL_INDICES_H

#include <opencv2/opencv.hpp>

namespace rs {
    // Normalized Difference Vegetation Index (Simulated using Red & Green)
    cv::Mat computeNDVI(const cv::Mat& src);
    
    // Normalized Difference Water Index (Simulated using Green & Red)
    cv::Mat computeNDWI(const cv::Mat& src);

    // Normalized Difference Snow Index (Simulated using Red & Blue)
    cv::Mat computeNDSI(const cv::Mat& src);
}

#endif // SPECTRAL_INDICES_H