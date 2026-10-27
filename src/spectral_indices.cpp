#include "spectral_indices.h"
#include <cmath>

namespace rs {

    // Helper function to apply a color map to the calculated index
    cv::Mat applyColormap(const cv::Mat& indexMap) {
        cv::Mat normalized, colored;
        // Normalize from -1.0 to 1.0 into 0 to 255 for visualization
        cv::normalize(indexMap, normalized, 0, 255, cv::NORM_MINMAX, CV_8U);
        cv::applyColorMap(normalized, colored, cv::COLORMAP_JET);
        return colored;
    }

    cv::Mat computeNDVI(const cv::Mat& src) {
        if (src.channels() < 3) return src.clone();
        
        std::vector<cv::Mat> channels;
        cv::split(src, channels);
        cv::Mat b = channels[0], g = channels[1], r = channels[2]; // BGR

        cv::Mat bF, gF, rF;
        b.convertTo(bF, CV_32F);
        g.convertTo(gF, CV_32F);
        r.convertTo(rF, CV_32F);

        // NDVI = (NIR - Red) / (NIR + Red). 
        // Simulated: (Green - Red) / (Green + Red + epsilon)
        cv::Mat indexMap = (gF - rF) / (gF + rF + 0.0001f);
        
        return applyColormap(indexMap);
    }

    cv::Mat computeNDWI(const cv::Mat& src) {
        if (src.channels() < 3) return src.clone();
        
        std::vector<cv::Mat> channels;
        cv::split(src, channels);
        cv::Mat b = channels[0], g = channels[1], r = channels[2];

        cv::Mat bF, gF, rF;
        b.convertTo(bF, CV_32F);
        g.convertTo(gF, CV_32F);
        r.convertTo(rF, CV_32F);

        // Simulated: (Blue - Red) / (Blue + Red)
        cv::Mat indexMap = (bF - rF) / (bF + rF + 0.0001f);
        
        return applyColormap(indexMap);
    }

    cv::Mat computeNDSI(const cv::Mat& src) {
        if (src.channels() < 3) return src.clone();
        
        std::vector<cv::Mat> channels;
        cv::split(src, channels);
        cv::Mat b = channels[0], g = channels[1], r = channels[2];

        cv::Mat bF, gF, rF;
        b.convertTo(bF, CV_32F);
        g.convertTo(gF, CV_32F);
        r.convertTo(rF, CV_32F);

        // Simulated: (Green - Blue) / (Green + Blue)
        cv::Mat indexMap = (gF - bF) / (gF + bF + 0.0001f);
        
        return applyColormap(indexMap);
    }
}