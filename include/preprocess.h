#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace rs {

cv::Mat loadImage(const std::string& path, int flags = cv::IMREAD_UNCHANGED);
bool saveImage(const std::string& path, const cv::Mat& img);
void printImageInfo(const cv::Mat& img, const std::string& label = "");
std::vector<cv::Mat> splitChannels(const cv::Mat& img);
cv::Mat mergeChannels(const std::vector<cv::Mat>& channels);
cv::Mat toGray(const cv::Mat& img);
cv::Mat normalizeMinMax(const cv::Mat& img);
cv::Mat resizeKeepAspect(const cv::Mat& img, int maxW, int maxH);
bool ensureParentDir(const std::string& filePath);

} // namespace rs

#endif // PREPROCESS_H