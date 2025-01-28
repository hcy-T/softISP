#include <iostream>
#include <string>
#include "SimpleISP.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <image_file>" << std::endl;
        return -1;
    }

    SimpleISP isp;

    int black_level = 240;
    
    // Read image
    if (!isp.readImage(argv[1])) {
        return -1;
    }

    std::string CFA;
    std::cout<< "Enter first pixel CFA, R,, Gr, Gb or B:";
    std::cin>> CFA;
    isp.getFirstPixelCFA(CFA);

    // 1. BLC
    if (!isp.BLC(black_level)){
        return -1;
    }

    //2. DGC
    if (!isp.DGC(4.0)){
        return -1;
    }

    //3.WB
    if (!isp.WB(1.073, 1.173)){
        return -1;
    }


    // 2. Convert to color (assuming BGGR pattern)
    if (!isp.convertToColor(cv::COLOR_BayerBG2BGR_EA)) {
        return -1;
    }

    // Display images
    std::cout<<"display raw image start"<<std::endl;
    isp.displayRaw();
    std::cout<<"display color image start"<<std::endl;    
    isp.displayColor();

    // Save the converted image
    std::string outputFilename = "converted_output.png";
    std::cout<<"save output image start"<<std::endl;
    if (!isp.saveImage(outputFilename)) {
        return -1;
    }

    std::cout << "Successfully processed and saved image to: " << outputFilename << std::endl;
    return 0;
}