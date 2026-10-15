#ifndef LINEAR_ENHANCE_H
#define LINEAR_ENHANCE_H

#include <opencv2/opencv.hpp>

namespace rs {

cv::Mat contrastStretch(const cv::Mat& src);

cv::Mat piecewiseLinearStretch(const cv::Mat& src,
                               double lowPercent  = 0.01,
                               double highPercent = 0.01);

cv::Mat histEqualize(const cv::Mat& src);

cv::Mat clahe(const cv::Mat& src,
              double clipLimit = 3.0,
              int    tileSize  = 8);

} // namespace rs

#endif // LINEAR_ENHANCE_H