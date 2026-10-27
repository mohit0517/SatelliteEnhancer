#include "linear_enhance.h"
#include <vector>
#include <algorithm>

namespace rs {

// ============================================================
//  Linear Min-Max Stretch
// ============================================================
cv::Mat contrastStretch(const cv::Mat& src) {
    cv::Mat out;
    std::vector<cv::Mat> chans;
    cv::split(src, chans);

    for (auto& c : chans) {
        double mn, mx;
        cv::minMaxLoc(c, &mn, &mx);
        if (mx - mn > 1e-9) {
            c.convertTo(c, CV_8U,
                        255.0 / (mx - mn),
                        -mn * 255.0 / (mx - mn));
        } else {
            c.convertTo(c, CV_8U);
        }
    }
    cv::merge(chans, out);
    return out;
}

// ============================================================
//  Piecewise Linear Stretch with Saturation
// ============================================================
cv::Mat piecewiseLinearStretch(const cv::Mat& src,
                               double lowPercent,
                               double highPercent) {
    cv::Mat out;
    std::vector<cv::Mat> chans;
    cv::split(src, chans);

    for (auto& c : chans) {
        cv::Mat hist;
        int histSize = 256;
        float range[] = {0, 256};
        const float* histRange = {range};
        cv::calcHist(&c, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

        double total = c.total();
        double lowCut  = total * lowPercent;
        double highCut = total * highPercent;

        int lowVal = 0, highVal = 255;
        double accum = 0;
        for (int i = 0; i < 256; ++i) {
            accum += hist.at<float>(i);
            if (accum > lowCut) { lowVal = i; break; }
        }
        accum = 0;
        for (int i = 255; i >= 0; --i) {
            accum += hist.at<float>(i);
            if (accum > highCut) { highVal = i; break; }
        }

        if (highVal - lowVal > 1) {
            c.convertTo(c, CV_8U,
                        255.0 / (highVal - lowVal),
                        -lowVal * 255.0 / (highVal - lowVal));
        }
    }
    cv::merge(chans, out);
    return out;
}

// ============================================================
//  Histogram Equalization (global)
// ============================================================
cv::Mat histEqualize(const cv::Mat& src) {
    cv::Mat out = src.clone();

    if (src.channels() == 1) {
        cv::equalizeHist(src, out);
    } else {
        // Equalize only the luminance channel (preserves color)
        cv::Mat ycrcb;
        cv::cvtColor(src, ycrcb, cv::COLOR_BGR2YCrCb);
        std::vector<cv::Mat> chans;
        cv::split(ycrcb, chans);
        cv::equalizeHist(chans[0], chans[0]);
        cv::merge(chans, ycrcb);
        cv::cvtColor(ycrcb, out, cv::COLOR_YCrCb2BGR);
    }
    return out;
}

// ============================================================
//  CLAHE
// ============================================================
cv::Mat clahe(const cv::Mat& src, double clipLimit, int tileSize) {
    cv::Ptr<cv::CLAHE> claheOp = cv::createCLAHE(clipLimit, cv::Size(tileSize, tileSize));
    cv::Mat out = src.clone();

    if (src.channels() == 1) {
        // Grayscale image
        claheOp->apply(src, out);
    } else {
        // Color image: Convert to LAB color space
        cv::Mat lab;
        cv::cvtColor(src, lab, cv::COLOR_BGR2Lab);

        // Split into L, A, B channels
        std::vector<cv::Mat> channels;
        cv::split(lab, channels);

        // Apply CLAHE ONLY to the L channel (Luminance)
        claheOp->apply(channels[0], channels[0]);

        // Merge back and convert to BGR
        cv::merge(channels, lab);
        cv::cvtColor(lab, out, cv::COLOR_Lab2BGR);
    }
    return out;
}

} // namespace rs