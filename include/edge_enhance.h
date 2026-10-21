#ifndef EDGE_ENHANCE_H
#define EDGE_ENHANCE_H

#include <opencv2/opencv.hpp>

namespace rs {

// Roberts Cross operator (2x2 kernel, fast but sensitive to noise)
cv::Mat robertsEdge(const cv::Mat& src);

// Sobel operator (3x3 kernel, gradient-based, very common)
cv::Mat sobelEdge(const cv::Mat& src);

// Laplacian operator (2nd derivative, detects edges in all directions)
cv::Mat laplacianEdge(const cv::Mat& src);

// Laplacian Sharpening (adds Laplacian back to original image)
cv::Mat laplacianSharpen(const cv::Mat& src);

} // namespace rs

#endif // EDGE_ENHANCE_H