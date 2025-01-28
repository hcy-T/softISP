#include "SimpleISP.h"
#include <iostream>
#include <cstdint>

#define DEBUG_MODE false

uint16_t maxClamp_uint16(float target, uint64_t max){
    return (target > max)? max: (uint16_t)std::round(target);    
}


void SimpleISP::getFirstPixelCFA(std::string CFA){
    first_pixel = CFA;
    std::cout<<"first pixel CFA = "<<first_pixel<<std::endl;
}

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

    getBayerChannel();
    if (DEBUG_MODE){
        std::cout << "Verification after processing:" << std::endl;
        std::cout << "R channel empty? " << bayer_ch1.empty() << std::endl;
        std::cout << "Gr channel empty? " << bayer_ch2.empty() << std::endl;
        std::cout << "Gb channel empty? " << bayer_ch3.empty() << std::endl;
        std::cout << "B channel empty? " << bayer_ch4.empty() << std::endl;       
    }
    return true;
}

bool SimpleISP::getBayerChannel(){
    if (rawImage.empty()) {
        std::cerr << "Error: No raw image loaded" << std::endl;
        return false;
    }
    int bitDepth = rawImage.depth();
    bayer_ch1 = cv::Mat(rawImage.rows/2, rawImage.cols/2, CV_MAKETYPE(bitDepth, 1));
    bayer_ch2 = cv::Mat(rawImage.rows/2, rawImage.cols/2, CV_MAKETYPE(bitDepth, 1));
    bayer_ch3 = cv::Mat(rawImage.rows/2, rawImage.cols/2, CV_MAKETYPE(bitDepth, 1));
    bayer_ch4 = cv::Mat(rawImage.rows/2, rawImage.cols/2, CV_MAKETYPE(bitDepth, 1));

    // Extract the color channels
    int ind_y = 0, ind_x = 0;
    for (int y = 0; y < rawImage.rows; y += 2) {
        ind_y = y/2;
        for (int x = 0; x < rawImage.cols; x += 2) {
            ind_x = x/2;

            // Print first few values for debugging
            if (DEBUG_MODE){
                if (y < 2 && x < 2) {
                    std::cout << "Position (" << y << "," << x << ") -> (" << ind_y << "," << ind_x << ")" << std::endl;
                    std::cout << "Raw values at this position:" << std::endl;
                    std::cout << "R: " << rawImage.at<uint16_t>(y, x) << std::endl;
                    std::cout << "Gr: " << rawImage.at<uint16_t>(y, x + 1) << std::endl;
                    std::cout << "Gb: " << rawImage.at<uint16_t>(y + 1, x) << std::endl;
                    std::cout << "B: " << rawImage.at<uint16_t>(y + 1, x + 1) << std::endl;
                }
            }

            switch (bitDepth) {
                case CV_8U:
                    bayer_ch1.at<uint8_t>(ind_y, ind_x) = rawImage.at<uint8_t>(y, x);
                    bayer_ch2.at<uint8_t>(ind_y, ind_x) = rawImage.at<uint8_t>(y, x + 1);
                    bayer_ch3.at<uint8_t>(ind_y, ind_x) = rawImage.at<uint8_t>(y + 1, x);
                    bayer_ch4.at<uint8_t>(ind_y, ind_x) = rawImage.at<uint8_t>(y + 1, x + 1);
                    break;
                case CV_16U:
                    bayer_ch1.at<uint16_t>(ind_y, ind_x) = rawImage.at<uint16_t>(y, x);
                    bayer_ch2.at<uint16_t>(ind_y, ind_x) = rawImage.at<uint16_t>(y, x + 1);
                    bayer_ch3.at<uint16_t>(ind_y, ind_x) = rawImage.at<uint16_t>(y + 1, x);
                    bayer_ch4.at<uint16_t>(ind_y, ind_x) = rawImage.at<uint16_t>(y + 1, x + 1);
                    break;
                default:
                    std::cerr << "Unsupported bit depth: " << bitDepth << std::endl;
                    break;
            }
            if (DEBUG_MODE){
                if (ind_y < 2 && ind_x < 2) 
                { 
                    std::cout << "Raw values at this position:" << std::endl;
                    std::cout << "R: " << bayer_ch1.at<uint16_t>(ind_y, ind_x) << std::endl;
                    std::cout << "Gr: " << bayer_ch2.at<uint16_t>(ind_y, ind_x) << std::endl;
                    std::cout << "Gb: " << bayer_ch3.at<uint16_t>(ind_y, ind_x) << std::endl;
                    std::cout << "B: " << bayer_ch4.at<uint16_t>(ind_y, ind_x) << std::endl;
                }
            }            
        }
    }
    return true; 
}

bool SimpleISP::BLC(const int black_level){
    if (rawImage.empty()) {
        std::cerr << "Error: No raw image loaded" << std::endl;
        return false;
    }
    cv::subtract(rawImage, black_level, rawImage);
    /*
    cv::threshold(InputArray src,     // Input image (rawImage)
             OutputArray dst,     // Output image (rawImage)
             double thresh,       // Threshold value (0)
             double maxval,       // Maximum value (0, not used with THRESH_TOZERO)
             int type)           // Thresholding type (THRESH_TOZERO)
    */
    cv::threshold(rawImage, rawImage, 0, 0, cv::THRESH_TOZERO);
    return true;
}

bool SimpleISP::DGC(const float dgc){
    if (rawImage.empty()) {
        std::cerr << "Error: No raw image loaded" << std::endl;
        return false;
    }
    uint64_t max = (1<<rawImage.depth()*8) - 1;
    for(int y = 0; y<rawImage.rows; y++){
        for (int x = 0; x < rawImage.cols; x++){
            rawImage.at<uint16_t>(y, x)  = maxClamp_uint16(rawImage.at<uint16_t>(y, x)*dgc, max);
        }
    }

    return true;
}

bool SimpleISP::WB(const float r_gain, const float b_gain){
    if (rawImage.empty()) {
        std::cerr << "Error: No raw image loaded" << std::endl;
        return false;
    }
    uint64_t max = (1<<rawImage.depth()*8) - 1;
    if (first_pixel == "R"){
        for (int y = 0; y < rawImage.rows; y += 2) {
            for (int x = 0; x < rawImage.cols; x += 2) {
                rawImage.at<uint16_t>(y, x)  = maxClamp_uint16(rawImage.at<uint16_t>(y, x)* r_gain, max);
                rawImage.at<uint16_t>(y+1, x+1) =maxClamp_uint16(rawImage.at<uint16_t>(y+1, x+1)* b_gain, max);
            }
        }
    }
    else if (first_pixel == "B"){
        for (int y = 0; y < rawImage.rows; y += 2) {
            for (int x = 0; x < rawImage.cols; x += 2) {
                rawImage.at<uint16_t>(y, x)  = maxClamp_uint16(rawImage.at<uint16_t>(y, x)* b_gain, max);
                rawImage.at<uint16_t>(y+1, x+1) =maxClamp_uint16(rawImage.at<uint16_t>(y+1, x+1)* r_gain, max);
            }
        }
    }
    else if (first_pixel == "Gr"){
        for (int y = 0; y < rawImage.rows; y += 2) {
            for (int x = 0; x < rawImage.cols; x += 2) {
                rawImage.at<uint16_t>(y, x+1)  = maxClamp_uint16(rawImage.at<uint16_t>(y, x+1)* r_gain, max);
                rawImage.at<uint16_t>(y+1, x) =maxClamp_uint16(rawImage.at<uint16_t>(y+1, x)* b_gain, max);
            }
        }
    }
    else if (first_pixel == "Gb"){
        for (int y = 0; y < rawImage.rows; y += 2) {
            for (int x = 0; x < rawImage.cols; x += 2) {
                rawImage.at<uint16_t>(y, x+1)  = maxClamp_uint16(rawImage.at<uint16_t>(y, x+1)* b_gain, max);
                rawImage.at<uint16_t>(y+1, x) =maxClamp_uint16(rawImage.at<uint16_t>(y+1, x)* r_gain, max);;
            }
        }
    }

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
