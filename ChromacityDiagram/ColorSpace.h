//
//  ColorSpace.hpp
//  split_toning
//
//  Created by Вадим on 04.12.16.
//  Copyright © 2016 Вадим. All rights reserved.
//

#ifndef ColorSpace_hpp
#define ColorSpace_hpp

#include <stdio.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>


namespace ColorSpace {
    
    template <typename T>
    struct Range {
        Range() : start(0), end(0) {}
        Range(T start_, T end_) : start(start_), end(end_) {}
        
        T start;
        T end;
    };
    
    typedef std::pair<cv::Point2f, cv::Point2f> LinePointsPair;
    
    
    
    /** @brief Creates Chromatic diagram CIE xy 
        
        @param size Size of mat to draw to
        @param range Range of normalized values to consider
        @param restricLinesPairs cut off zones specified by lines. Lines are specified by points pair
        @return Returns mat of specified size with 3 channels BGR of float type (0...1)
     */
    cv::Mat createChromaticDiagram(cv::Size size,
                                   const Range<float> &range,
                                   const std::vector<LinePointsPair> &restrictLinesPairs);
    

    
    
    
    /** @brief Draws Planckian Locus
     
        The function draws planckian locus into a chromatic space xy.
        @param outputMat Mat to draw to (chromatic space)
        @param range Range of normalized values to consider
        @param temperaturesRange Range of temperature to draw
        @param showValues Array of values for showing its values on the graphics (empty dy default)
     */
    void drawPlanckianLocus(cv::Mat &outputMat,
                            const Range<float> &range,
                            cv::Range temperaturesRange,
                            std::vector<int> showValues = std::vector<int>());
    
    
    
    
    
    /** @brief Draws grid
        
        The function draws grid
        @param outputMat Mat to draw to
        @param range Range of normalized values to consider
        @param stepSize Size of grid step
        @param gridColor Color of grid lines
     */
    void drawGrid(cv::Mat &outputMat, const Range<float> &range, float stepSize, cv::Vec3f gridColor);

}


#endif /* ColorSpace_hpp */
