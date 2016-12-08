//
//  ColorSpace.cpp
//  split_toning
//
//  Created by Вадим on 04.12.16.
//  Copyright © 2016 Вадим. All rights reserved.
//

#include "ColorSpace.h"


namespace ColorSpace {
    using namespace cv;
    

    
    /// Approximates planckian locus points for temperatures
    Point2f getPlanckianLocusPointInXYspace(int temperature);
    
    

    Mat createChromaticDiagram(cv::Size size, const Range<float> &range, const std::vector<LinePointsPair> &restrictLinesPairs) {
        
        Mat colorSpaceMat(size.height, size.width, CV_32FC3);
        Point2f centerPoint(0.33, 0.33);
        //Vec3f restrcitedZoneColor(0.35, 0.35, 0.35);
        Vec3f backgroundColor(0.05,0.05,0.05);
        float rangeAmpl = range.end - range.start;
        
        
        //# Compute restricted lines normals
        typedef std::pair<Point2f, Vec2f> LineParams;
        std::vector<LineParams> linesVector;
        for (auto linePair : restrictLinesPairs) {
            Point2f diffVect = linePair.second - linePair.first;
            Vec2f normal(-diffVect.y, diffVect.x);
            Vec2f diff(centerPoint - linePair.first);
            if (diff.dot(normal) > 0) // all normal must have direction out from center of diagram
                normal *= -1;
            linesVector.push_back(LineParams {linePair.first, normal} );
        }
        
        
        //# Iterate x,y - compute values in XYZ space & restrict zones
        for (int y = 0; y < size.height; y++) {
            for (int x = 0 ; x < size.width; x++) {
                
                // normalize
                Point2f xyPoint((float) x / (size.width - 1), (float) y / (size.height - 1));
                // range correction
                xyPoint *= rangeAmpl;
                xyPoint.x += range.start;
                xyPoint.y += range.start;

                // analyze for restircted zone pixels
                bool skip = false;
                for (auto lineParams : linesVector) {
                    Vec2f diffVec(xyPoint - lineParams.first);
                    if (diffVec.dot(lineParams.second) >= 0) {
                        skip = true;
                        break;
                    }
                }
                
                int invertred_y = size.height - 1  - y; // 0 at buttom
                
                if (skip)
                    //### SKIP if point is rectricted by line
                    colorSpaceMat.at<Vec3f>(invertred_y, x) = Vec3f(0,0,0);
 
                else
                    colorSpaceMat.at<Vec3f>(invertred_y, x) = Vec3f(xyPoint.x, xyPoint.y, 1 - xyPoint.x - xyPoint.y);
                
                
            }
        }
        
        
        // Convert spaces XYZ -> RGB
        cvtColor(colorSpaceMat, colorSpaceMat, COLOR_XYZ2BGR);

        
        // Remove colors with negative components
        for (int y = 0; y < size.height; y++) {
            for (int x = 0 ; x < size.width; x++) {
                Vec3f &color = colorSpaceMat.at<Vec3f>(y, x);
                if (color[0] < 0 || color[1] < 0 || color[2] < 0 || color == Vec3f(0,0,0))
                    color = backgroundColor;
                
                else {
                    float maxValue = std::max(std::max(color[0], color[1]), color[2]);
                    color /= maxValue;
                }
            }
        }

        // gamma correction
        cv::pow(colorSpaceMat, 1.0/ 2.4, colorSpaceMat);
        
        return colorSpaceMat;
    }

    
    
    
    
    void drawGrid(cv::Mat &outputMat, const Range<float> &range, float stepSize, cv::Vec3f gridColor) {
        
        float stepsCount = ((range.end - range.start) / stepSize);
        int width = outputMat.size().width;
        int height = outputMat.size().height;
        int stepWidth = width / stepsCount;
        int stepHeight = height / stepsCount;
        float value = range.start;
        char buff[5];
        
        for(int i = height; i > 0; i -= stepHeight) {
            cv::line(outputMat, Point(0, i), Point(width, i), gridColor);
            std::sprintf(buff, "%.2f", value);
            cv::putText(outputMat, buff, Point(0, i - 1), cv::FONT_HERSHEY_SIMPLEX, 0.7, Scalar(1,1,1), 1, LINE_AA, false);
            value += stepSize;
        }
        
        value = range.start;
        for(int i = 0; i < width; i += stepWidth) {
            std::sprintf(buff, "%.2f", value);
            cv::line(outputMat, Point(i,0),Point(i,height), gridColor);
            if (value != range.start) // skip first value
                cv::putText(outputMat, buff, Point(i, height - 1), cv::FONT_HERSHEY_SIMPLEX, 0.7, Scalar(1,1,1), 1, LINE_AA, false);
            value += stepSize;
        }
        
        
    }
    
    
    
    
    
    void drawPlanckianLocus(cv::Mat &outputMat, const Range<float> &range, cv::Range temperaturesRange, std::vector<int> showValues) {
        
        cv::Point prevPoint(0,0);
        cv::Size size = outputMat.size();
        
        // nested function for converting coords
        auto convert_XY_PointToScreen = [&](cv::Point2f &point) {
            cv::Point2f convertedPoint = point;
            convertedPoint.x -= range.start;
            convertedPoint.y -= range.start;
            convertedPoint /= (range.end - range.start);
            // convert to size
            convertedPoint.x *= size.width;
            convertedPoint.y = (1 - convertedPoint.y);
            convertedPoint.y *= size.height;
            return  convertedPoint;
        };
        
        // Drawing cycle
        for (int temperature = temperaturesRange.start; temperature <= temperaturesRange.end; temperature += 100) {
            
            cv::Point2f planckianLocusPoint = getPlanckianLocusPointInXYspace(temperature);
            planckianLocusPoint = convert_XY_PointToScreen(planckianLocusPoint);
            if (prevPoint.x > 0 && prevPoint.y > 0) // skip first zero point
                cv::line(outputMat, prevPoint, planckianLocusPoint, Scalar(0,0,0), 1);
           
            prevPoint = planckianLocusPoint;
        }
        
        // Draw some specified points
        if (!showValues.empty()) {
            char buff[5];
            for (auto temperature : showValues) {
                std::sprintf(buff, "D%d", temperature / 100);
                cv::Point2f planckianLocusPoint = getPlanckianLocusPointInXYspace(temperature);
                planckianLocusPoint = convert_XY_PointToScreen(planckianLocusPoint);
                cv::circle(outputMat, planckianLocusPoint, 2, Scalar(0,0,0), -1);
                cv::putText(outputMat, buff, planckianLocusPoint + Point2f(2,2), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0.1,.1,.1));
            }
        }

    }
    
    
    
    
    
    /**  Referrance: https://en.wikipedia.org/wiki/Planckian_locus */
    Point2f getPlanckianLocusPointInXYspace(int temperature) {
        Point2f point;
        
        static const Vec4f coefsX_1667K_4000K(-0.2661239  * 1E+9, -0.2343580 * 1E+6, 0.8776956 * 1E+3, 0.179910);
        static const Vec4f coefsX_4000K_25000K(-3.0258469 * 1E+9,  2.1070379 * 1E+6, 0.2226347 * 1E+3, 0.240390);
        
        static const Vec4f coefsY_1667K_2222K(-1.1063814, -1.34811020, 2.18555832, -0.20219683);
        static const Vec4f coefsY_2222K_4000K(-0.9549476, -1.37418593, 2.09137015, -0.16748867);
        static const Vec4f coefsY_4000K_25000K(3.0817580, -5.87338670, 3.75112997, -0.37001483);
        
        Vec4f temperatures(1.0 / std::powf(temperature, 3),
                           1.0 / std::powf(temperature, 2),
                           1.0 / temperature,
                           1);
        
        
        if (temperature <= 4000)
            point.x = coefsX_1667K_4000K.dot(temperatures);
        
        else if (temperature <= 25000)
            point.x = coefsX_4000K_25000K.dot(temperatures);
        
        
        Vec4f x_pows(std::pow(point.x, 3), std::pow(point.x, 2), point.x, 1);
        
        if (temperature <= 2222)
            point.y = coefsY_1667K_2222K.dot(x_pows);
        
        else if (temperature <= 4000)
            point.y = coefsY_2222K_4000K.dot(x_pows);
        
        else if (temperature <= 25000)
            point.y = coefsY_4000K_25000K.dot(x_pows);
        
        
        return point;
        
    }
    
}