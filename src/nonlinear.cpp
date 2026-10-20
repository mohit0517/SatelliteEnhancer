#include "nonlinear.h"
#include <vector>
#include <cmath>

namespace rs {

// ============================================================
//  Gamma Correction
//  s = 255 * (r / 255)^gamma
// ============================================================
cv::Mat gammaCorrect(const cv::Mat& src, double gamma) {
    cv::Mat lut(1, 256, CV_8U);
    for (int i = 0; i < 256; ++i) {
        lut.at<uchar>(i) = cv::saturate_cast<uchar>(
            std::pow(i / 255.0, gamma) * 255.0);
    }
    cv::Mat out;
    cv::LUT(src, lut, out);
    return out;
}

// ============================================================
//  Logarithmic Transform
//  s = c * log(1 + r)   where c = 255 / log(1 + max)
// ============================================================
cv::Mat logTransform(const cv::Mat& src) {
    cv::Mat srcF;
    src.convertTo(srcF, CV_32F);

    double minVal, maxVal;
    cv::minMaxLoc(srcF, &minVal, &maxVal);

    double c = 255.0 / std::log(1.0 + maxVal);

    cv::Mat out(src.size(), src.type());
    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            if (src.channels() == 1) {
                float v = srcF.at<float>(y, x);
                out.at<uchar>(y, x) = cv::saturate_cast<uchar>(
                    c * std::log(1.0 + v));
            } else {
                cv::Vec3f v = srcF.at<cv::Vec3f>(y, x);
                cv::Vec3b o;
                for (int c2 = 0; c2 < 3; ++c2) {
                    o[c2] = cv::saturate_cast<uchar>(
                        c * std::log(1.0 + v[c2]));
                }
                out.at<cv::Vec3b>(y, x) = o;
            }
        }
    }
    return out;
}

// ============================================================
//  Inverse Log (Exponential)
//  s = c * (exp(r/k) - 1)   with k tuned so max maps to 255
// ============================================================
cv::Mat inverseLogTransform(const cv::Mat& src) {
    cv::Mat srcF;
    src.convertTo(srcF, CV_32F);

    double minVal, maxVal;
    cv::minMaxLoc(srcF, &minVal, &maxVal);

    double k = maxVal / std::log(256.0);   // maps max → 255 after transform
    if (k < 1e-6) k = 1.0;

    cv::Mat out(src.size(), src.type());
    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            if (src.channels() == 1) {
                float v = srcF.at<float>(y, x);
                out.at<uchar>(y, x) = cv::saturate_cast<uchar>(
                    (std::exp(v / k) - 1.0));
            } else {
                cv::Vec3f v = srcF.at<cv::Vec3f>(y, x);
                cv::Vec3b o;
                for (int c2 = 0; c2 < 3; ++c2) {
                    o[c2] = cv::saturate_cast<uchar>(
                        (std::exp(v[c2] / k) - 1.0));
                }
                out.at<cv::Vec3b>(y, x) = o;
            }
        }
    }
    return out;
}

} // namespace rs