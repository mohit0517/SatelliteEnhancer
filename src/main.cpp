#include "preprocess.h"
#include "linear_enhance.h"
#include <opencv2/core/utils/logger.hpp>

#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    // Silence OpenCV's INFO/parallel-backend warnings
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    std::string rawDir      = (argc > 1) ? argv[1] : "data/raw";
    std::string enhancedDir = (argc > 2) ? argv[2] : "data/enhanced";

    std::cout << "============================================\n";
    std::cout << " Satellite Image Enhancement - Week 2\n";
    std::cout << "============================================\n";
    std::cout << " Input  : " << rawDir      << "\n";
    std::cout << " Output : " << enhancedDir << "\n\n";

    if (!fs::exists(rawDir)) {
        std::cerr << "ERROR: input folder not found: " << rawDir << "\n";
        return 1;
    }

    fs::create_directories(enhancedDir);

    std::vector<std::string> exts = {".png", ".tif", ".tiff", ".jpg", ".jpeg", ".bmp"};
    int processed = 0, failed = 0;

    for (const auto& entry : fs::directory_iterator(rawDir)) {
        if (!entry.is_regular_file()) continue;

        std::string ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        bool supported = false;
        for (const auto& e : exts)
            if (ext == e) { supported = true; break; }
        if (!supported) continue;

        const std::string path = entry.path().string();
        const std::string stem = entry.path().stem().string();

        std::cout << "Processing: " << entry.path().filename().string() << "\n";

        try {
            cv::Mat img = rs::loadImage(path);
            rs::printImageInfo(img, "Original");

            // ---------- Week 1: Preprocessing ----------
            cv::Mat gray  = rs::toGray(img);
            rs::saveImage(enhancedDir + "/" + stem + "_gray.png",  gray);

            cv::Mat small = rs::resizeKeepAspect(img, 1024, 1024);
            rs::saveImage(enhancedDir + "/" + stem + "_small.png", small);

            // ---------- Week 2: Linear Enhancement ----------
            cv::Mat stretch = rs::contrastStretch(img);
            rs::saveImage(enhancedDir + "/" + stem + "_stretch.png", stretch);

            cv::Mat piecewise = rs::piecewiseLinearStretch(img, 0.01, 0.01);
            rs::saveImage(enhancedDir + "/" + stem + "_piecewise.png", piecewise);

            cv::Mat histEq = rs::histEqualize(img);
            rs::saveImage(enhancedDir + "/" + stem + "_histeq.png", histEq);

            cv::Mat claheImg = rs::clahe(img, 3.0, 8);
            rs::saveImage(enhancedDir + "/" + stem + "_clahe.png", claheImg);

            std::cout << "  -> saved: gray, small, stretch, piecewise, histeq, clahe\n\n";
            ++processed;
        }
        catch (const std::exception& e) {
            std::cerr << "  FAILED: " << e.what() << "\n\n";
            ++failed;
        }
    }

    std::cout << "============================================\n";
    std::cout << " Done. Processed: " << processed
              << "   Failed: " << failed << "\n";
    std::cout << " Outputs in: " << enhancedDir << "\n";
    std::cout << "============================================\n";
    return failed == 0 ? 0 : 1;
}