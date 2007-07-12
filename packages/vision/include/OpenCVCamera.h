/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim and Joseph Lisee
 * File:  packages/vision/include/OpenCVCamera.h
 */

#ifndef RAM_VISION_OPENCVCAMERA_H_06_06_2007
#define RAM_VISION_OPENCVCAMERA_H_06_06_2007
#include <string>

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Camera.h"

// Forward Declartions
struct CvCapture; // For OpenCV

namespace ram {
namespace vision {

class OpenCVCamera : public Camera
{
public:
	OpenCVCamera();
    OpenCVCamera(int camNum, bool forward);
	OpenCVCamera(std::string movieName);
    ~OpenCVCamera();

    /** This grabs the new image, and then stores it for Camera::getImage */
    virtual void update(double timestep);
	
	virtual void getUncalibratedImage(Image* distorted);
	
	virtual void getImage(Image* undistorted);
	
	/** Retrieves the latest image from camera and undistorts it into undistorted
	*	@undistorted the image to be undistorted
	*/
	void getCalibratedImage(Image* undistorted);

    virtual size_t width();
    
    virtual size_t height();

private:
	/*A Calibration with parameters that can be set for this camera*/
	Calibration* m_calibration;
    CvCapture* m_camCapture;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_OPENCVCAMERA_H_06_06_2007
