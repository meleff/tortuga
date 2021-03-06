/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/DownwardDuctDetector.cpp
 */

// STD Includes

// Library Includes

// Project Includes
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/DownwardDuctDetector.h"
#include "vision/include/Events.h"
#include "vision/include/main.h"

namespace ram {
namespace vision {

int DownwardDuctDetector::yellow(unsigned char r, unsigned char g, unsigned char b, double minRedOverGreen, double maxRedOverGreen, double minRedOverBlue, double minGreenOverBlueOnFailure, int maxRedFailureThresh, int minTotalRGB, int minTotalRGBOnFailure)
{
    //Yellow
    if ((g * minRedOverGreen <= r) &&
        (g * maxRedOverGreen >= r) &&
        (b * minRedOverBlue <= r) &&
        (r + b + g >= minTotalRGB))
    {
        return 1;
    }
    else if ((r <= maxRedFailureThresh) && (r < b) &&
             (g >= (b * minGreenOverBlueOnFailure)) &&
             (r+b+g >= minTotalRGBOnFailure))
    {//Dark greenish yellowish blackish stuff
        return 1;
    }
    return 0;
}

DownwardDuctDetector::DownwardDuctDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_working(new OpenCVImage(640, 480)),
    m_x(0.0),
    m_y(0.0),
    m_found(false)
{
    init(config);
}
    
DownwardDuctDetector::DownwardDuctDetector(core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_working(new OpenCVImage(640, 480)),
    m_x(0.0),
    m_y(0.0),
    m_found(false)
{
    init(core::ConfigNode::fromString("{}"));
}

DownwardDuctDetector::~DownwardDuctDetector()
{
    delete m_working;
}

void DownwardDuctDetector::init(core::ConfigNode config)
{
    // get the threshold values from the config file
    m_minRedOverGreen = config["minRedOverGreen"].asDouble(0.5);
    m_maxRedOverGreen = config["maxRedOverGreen"].asDouble(1.5);
    m_minRedOverBlue = config["minRedOverBlue"].asDouble(1.0);
    m_minGreenOverBlueOnRedFailureForInsideDuct =
        config["minGreenOverBlueOnRedFailureForInsideDuct"].asDouble(1.1);
    
    m_maxRedFailureThresh = config["maxRedFailureThresh"].asInt(50);
    m_minTotalRGB = config["minTotalRGB"].asInt(125);
    m_minTotalRGBOnFailure = config["minTotalRGBOnFailure"].asInt(150);
}
    
void DownwardDuctDetector::processImage(Image* input, Image* output)
{   
    m_working->copyFrom(input);

    // Grab data pointers
    unsigned char* data = (unsigned char *)(m_working->asIplImage()->imageData);

    int width = m_working->getWidth();
    int height = m_working->getHeight();

    long totalX = 0;
    long totalY = 0;
    int yellowPixelCount = 0;
    int count = 0;
    for (int y = 0; y < height; y ++)
    {
        for (int x = 0; x < width; x++)
        {
            if (yellow(data[count+2],data[count+1],data[count],
		       m_minRedOverGreen, m_maxRedOverGreen, m_minRedOverBlue,
		       m_minGreenOverBlueOnRedFailureForInsideDuct,
		       m_maxRedFailureThresh, m_minTotalRGB, 
		       m_minTotalRGBOnFailure))
            {
                data[count] = 255;
                data[count+1] = 255;
                data[count+2] = 255;
                totalX += x;
                totalY += y; 
                yellowPixelCount++;            
            }
            else
            {
                data[count] = data[count+1] = data[count+2] = 0;
            }
            count+=3;
        }
    }
    //for debug
    if (output)
    {
        output->copyFrom(m_working);
    }    
    
    if (yellowPixelCount >= 100)
    {
        m_x = (double)totalX / (double)yellowPixelCount;
        m_y = (double)totalY / (double)yellowPixelCount;

        if (output)
        {
            CvPoint ductCenter;
            ductCenter.x = (int)m_x;
            ductCenter.y = (int)m_y;
            cvCircle(output->asIplImage(), ductCenter, 10, CV_RGB(0,255,0), 2, CV_AA, 0);
        }
            
        // convert to the crazy coordinate system
        double n_x = -1 * ((width / 2) - m_x);
        double n_y = (height / 2) - m_y;
        n_x = n_x / ((double)width) * 2.0;
        n_y = n_y / ((double)height) * 2.0;
        n_x *= (double)width/height;
        
        m_found = true;
        // publish found event
        DuctEventPtr event(new DuctEvent(n_x, n_y, 0, 0, 
            false, false));
        publish(EventType::DOWN_DUCT_FOUND, event);
    }
    else
    {
        // if we had sight of it last time, but not this time, throw lost event
        if (m_found)
        {
            DuctEventPtr event(new DuctEvent(0, 0, 0, 0, false, false));
            publish(EventType::DOWN_DUCT_LOST, event);
            m_found = false;
        }
    }
}

} // namespace vision
} // namespace ram

