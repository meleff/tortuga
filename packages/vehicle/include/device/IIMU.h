/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IMU.h
 */

#ifndef RAM_VEHICLE_DEVICE_IMU_06_25_2007
#define RAM_VEHICLE_DEVICE_IMU_06_25_2007

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"

#include "math/include/Quaternion.h"

#include "pattern/include/Subject.h"


namespace ram {
namespace vehicle {
namespace device {
    
class IMU : public IDevice,          // For getName
            public pattern::Subject // so other objects can watch
            // boost::noncopyable
{
public:
    enum UpdateEvents {
        DataUpdate
    };
        
    virtual ~IIMU() {};

    math::Quaternion getOrientation();
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IMU_06_25_2007
