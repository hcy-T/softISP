#pragma once
#ifndef SIMPLE_ISP_H
#define SIMPLE_ISP_H

#include <opencv2/opencv.hpp>
#include <string>

class SimpleISP {
private:
    cv::Mat rawImage;
    cv::Mat colorImage;
    cv::Mat bayer_ch1;
    cv::Mat bayer_ch2;
    cv::Mat bayer_ch3;
    cv::Mat bayer_ch4;
    std::string first_pixel;
    uint16_t maxClamp(float target, uint64_t max);

public:
    bool readImage(const std::string& filename);
    bool getBayerChannel();
    bool BLC(const int black_level);
    bool DGC(const float dgc);
    bool WB(const float r_gain, const float b_gain);
    bool GammaCorrection(const float gamma);
    bool convertToColor(int bayerPattern = cv::COLOR_BayerBG2BGR);
    bool saveImage(const std::string& filename);
    void displayRaw();
    void displayColor();
    void getFirstPixelCFA(std::string);
};

#endif // SIMPLE_ISP_H
