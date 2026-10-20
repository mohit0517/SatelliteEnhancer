#include "preprocess.h"
#include "linear_enhance.h"
#include "nonlinear.h"
#include "filters.h"
#include <opencv2/core/utils/logger.hpp>

#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    std::string rawDir      = (argc > 1) ? argv[1] : "data/raw";
    std::string enhancedDir = (argc > 2) ? argv[2] : "data/enhanced";

    std::cout << "============================================\n";
    std::cout << " Satellite Image Enhancement - Week 3\n";
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

            std::string base = enhancedDir + "/" + stem;

            // ---------- Week 1: Preprocessing ----------
            rs::saveImage(base + "_gray.png",  rs::toGray(img));
            rs::saveImage(base + "_small.png", rs::resizeKeepAspect(img, 1024, 1024));

            // ---------- Week 2: Linear ----------
            rs::saveImage(base + "_stretch.png",   rs::contrastStretch(img));
            rs::saveImage(base + "_piecewise.png", rs::piecewiseLinearStretch(img, 0.01, 0.01));
            rs::saveImage(base + "_histeq.png",    rs::histEqualize(img));
            rs::saveImage(base + "_clahe.png",     rs::clahe(img, 3.0, 8));

            // ---------- Week 3: Non-linear ----------
            rs::saveImage(base + "_gamma_0.3.png", rs::gammaCorrect(img, 0.3));
            rs::saveImage(base + "_gamma_0.5.png", rs::gammaCorrect(img, 0.5));
            rs::saveImage(base + "_gamma_1.5.png", rs::gammaCorrect(img, 1.5));
            rs::saveImage(base + "_gamma_2.0.png", rs::gammaCorrect(img, 2.0));
            rs::saveImage(base + "_log.png",       rs::logTransform(img));
            rs::saveImage(base + "_invlog.png",    rs::inverseLogTransform(img));

            // ---------- Week 3: Spatial filters ----------
            rs::saveImage(base + "_median_3.png",  rs::medianFilter(img, 3));
            rs::saveImage(base + "_median_5.png",  rs::medianFilter(img, 5));
            rs::saveImage(base + "_gauss_5.png",   rs::gaussianFilter(img, 5, 1.5));
            rs::saveImage(base + "_highpass.png",  rs::highPassFilter(img, 9, 10.0));
            rs::saveImage(base + "_unsharp.png",   rs::unsharpMask(img, 5, 1.0, 1.5));
            rs::saveImage(base + "_bilateral.png", rs::bilateralFilter(img, 9, 75, 75));

            std::cout << "  -> saved 19 enhanced versions\n\n";
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