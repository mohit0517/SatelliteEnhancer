#include "preprocess.h"

#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

namespace rs {

cv::Mat loadImage(const std::string& path, int flags) {
    if (!fs::exists(path)) {
        throw std::runtime_error("File does not exist: " + path);
    }
    cv::Mat img = cv::imread(path, flags);
    if (img.empty()) {
        throw std::runtime_error(
            "Failed to read image (unsupported format or corrupt): " + path);
    }
    return img;
}

bool saveImage(const std::string& path, const cv::Mat& img) {
    if (img.empty()) return false;
    ensureParentDir(path);
    return cv::imwrite(path, img);
}

void printImageInfo(const cv::Mat& img, const std::string& label) {
    if (!label.empty()) std::cout << "--- " << label << " ---\n";
    std::cout << "  Size     : " << img.cols << " x " << img.rows << "\n";
    std::cout << "  Channels : " << img.channels() << "\n";
    std::cout << "  Depth    : " << img.depth() << "\n";

    std::vector<cv::Mat> chans;
    cv::split(img, chans);
    for (size_t c = 0; c < chans.size(); ++c) {
        double mn, mx;
        cv::minMaxLoc(chans[c], &mn, &mx);
        std::cout << "  Ch " << c << " min/max : " << mn << " / " << mx << "\n";
    }
    std::cout << std::endl;
}

std::vector<cv::Mat> splitChannels(const cv::Mat& img) {
    std::vector<cv::Mat> chans;
    cv::split(img, chans);
    return chans;
}

cv::Mat mergeChannels(const std::vector<cv::Mat>& channels) {
    cv::Mat out;
    cv::merge(channels, out);
    return out;
}

cv::Mat toGray(const cv::Mat& img) {
    if (img.channels() == 1) return img.clone();
    cv::Mat gray;
    if (img.channels() == 3)
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else if (img.channels() == 4)
        cv::cvtColor(img, gray, cv::COLOR_BGRA2GRAY);
    else {
        std::vector<cv::Mat> chans;
        cv::split(img, chans);
        cv::Mat accum = cv::Mat::zeros(img.size(), CV_32F);
        int n = std::min<int>(3, (int)chans.size());
        for (int i = 0; i < n; ++i) {
            cv::Mat f; chans[i].convertTo(f, CV_32F);
            accum += f;
        }
        accum /= n;
        accum.convertTo(gray, CV_8U);
    }
    return gray;
}

cv::Mat normalizeMinMax(const cv::Mat& img) {
    cv::Mat out;
    img.convertTo(out, CV_8U, 1.0);

    std::vector<cv::Mat> chans;
    cv::split(out, chans);

    for (auto& c : chans) {
        double mn, mx;
        cv::minMaxLoc(c, &mn, &mx);
        if (mx - mn > 1e-9) {
            c.convertTo(c, CV_8U,
                        255.0 / (mx - mn),
                        -mn * 255.0 / (mx - mn));
        }
    }

    cv::Mat result;
    cv::merge(chans, result);
    return result;
}

cv::Mat resizeKeepAspect(const cv::Mat& img, int maxW, int maxH) {
    if (img.cols <= maxW && img.rows <= maxH) return img.clone();
    double sx = (double)maxW / img.cols;
    double sy = (double)maxH / img.rows;
    double s  = std::min(sx, sy);
    cv::Mat out;
    cv::resize(img, out, cv::Size(), s, s, cv::INTER_AREA);
    return out;
}

bool ensureParentDir(const std::string& filePath) {
    fs::path p(filePath);
    fs::path parent = p.parent_path();
    if (!parent.empty() && !fs::exists(parent)) {
        std::error_code ec;
        fs::create_directories(parent, ec);
        return !ec;
    }
    return true;
}

} // namespace rs