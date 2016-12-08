//
//  main.cpp
//  ChromacityDiagram
//
//  Created by Вадим on 05.12.16.
//  Copyright © 2016 Вадим. All rights reserved.
//

#include <iostream>
#include "ColorSpace.h"


int main(int argc, const char * argv[]) {


    // Input parameters
    ColorSpace::Range<float> diagramRange(0, 0.7);
    cv::Size size(1024, 1024);
    std::vector<ColorSpace::LinePointsPair> restrictLines = {
        ColorSpace::LinePointsPair { cv::Point2f(0.8, 0.2), cv::Point2f(0.1, 0.9) },
        ColorSpace::LinePointsPair { cv::Point2f(0.172, 0), cv::Point2f(0.74, 0.26) },
    };
    
    
    // Draw
    cv::Mat xyColorSpace = ColorSpace::createChromaticDiagram(size, diagramRange, restrictLines);
    ColorSpace::drawPlanckianLocus(xyColorSpace, diagramRange, cv::Range(1500, 10000), std::vector<int> {2000, 4000, 5500, 6500, 9300});
    ColorSpace::drawGrid(xyColorSpace, diagramRange, 0.05, cv::Vec3f(0.45, 0.45, 0.45));
    imshow("Chromacity Diagram CIE xy (with planckian locus)", xyColorSpace);
    
    // Save
    cv::Mat saveColorSpace(size, CV_8UC3);
    xyColorSpace *= 255;
    xyColorSpace.convertTo(saveColorSpace, CV_8UC3);
    imwrite("chromatic.ppm", saveColorSpace);

    cv::waitKey();
    return 0;
}
