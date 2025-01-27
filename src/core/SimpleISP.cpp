#include "SimpleISP.h"
#include <iostream>

bool SimpleISP::readImage(const std::string& filename) {
    // Read image with original bit depth
    rawImage = cv::imread(filename, cv::IMREAD_UNCHANGED);
    if (rawImage.empty()) {
        std::cerr << "Error: Could not read image file: " << filename << std::endl;
        return false;
    }
    
    std::cout << "Image loaded successfully:" << std::endl;
    std::cout << "Width: " << rawImage.cols << std::endl;
    std::cout << "Height: " << rawImage.rows << std::endl;
    std::cout << "Channels: " << rawImage.channels() << std::endl;
    std::cout << "Depth: " << rawImage.depth()*8 << "-bit" << std::endl;
    
    return true;
}

bool SimpleISP::convertToColor(int bayerPattern) {
    if (rawImage.empty()) {
        std::cerr << "Error: No raw image loaded" << std::endl;
        return false;
    }

    try {
        cv::cvtColor(rawImage, colorImage, bayerPattern);
        return true;
    }
    catch (const cv::Exception& e) {
        std::cerr << "OpenCV error: " << e.what() << std::endl;
        return false;
    }
}

bool SimpleISP::saveImage(const std::string& filename) {
    if (colorImage.empty()) {
        std::cerr << "Error: No color image available" << std::endl;
        return false;
    }

    try {
        return cv::imwrite(filename, colorImage);
    }
    catch (const cv::Exception& e) {
        std::cerr << "Error saving image: " << e.what() << std::endl;
        return false;
    }
}

void SimpleISP::displayRaw() {
    if (!rawImage.empty()) {
        // If it's 16-bit, normalize for display
        if (rawImage.depth() == CV_16U) {
            cv::Mat displayImage;
            cv::normalize(rawImage, displayImage, 0, 65535, cv::NORM_MINMAX);
            // because cv::imshow only support 8-bit display
            displayImage.convertTo(displayImage, CV_8U, 255.0/65535.0);
            cv::imshow("Raw Image", displayImage);
        } 
        else {
            cv::imshow("Raw Image", rawImage);
        }
        std::cerr << "press any key to show color image"<< std::endl;
        cv::waitKey(0);
    }
}

void SimpleISP::displayColor() {
    if (!colorImage.empty()) {
        cv::imshow("Color Image", colorImage);
        std::cerr << "press any key to save color image"<< std::endl;
        cv::waitKey(0);
    }
}
