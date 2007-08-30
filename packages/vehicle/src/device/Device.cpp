/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/Thruster.cpp
 */

// STD Includes
#include <sstream>

// Project Includes
#include "vehicle/include/device/Device.h"

namespace ram {
namespace vehicle {
namespace device {

Device::Device(IVehicle* vehicle, std::string name) :
    m_name(name)//,
//    m_vehicle(vehicle)
{
}

std::string Device::getName()
{
    return m_name;
}

/*
IVehicle* Device::getVehicle()
{
    return m_vehicle;
    }*/
    
} // namespace device
} // namespace vehicle
} // namespace ram
