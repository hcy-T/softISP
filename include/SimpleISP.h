#pragma once
#ifndef SIMPLE_ISP_H
#define SIMPLE_ISP_H

#include <opencv2/opencv.hpp>
#include <string>

class SimpleISP {
private:
    cv::Mat rawImage;
    cv::Mat colorImage;

public:
    bool readImage(const std::string& filename);
    bool convertToColor(int bayerPattern = cv::COLOR_BayerBG2BGR);
    bool saveImage(const std::string& filename);
    void displayRaw();
    void displayColor();
};

#endif // SIMPLE_ISP_H
