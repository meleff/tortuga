/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Common.h
 */

#ifndef RAM_VEHICLE_COMMON_06_11_2006
#define RAM_VEHICLE_COMMON_06_11_2006

// STD Includes
#include <string>
#include <map>

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "vehicle/include/device/Common.h"

namespace ram {
namespace vehicle {

// Forward Declartions
class IVehicle;
typedef boost::shared_ptr<IVehicle> IVehiclePtr;
    
// Typedefs
typedef std::map<std::string, device::IDevicePtr> NameDeviceMap;
typedef NameDeviceMap::iterator NameDeviceMapIter;
typedef std::vector<std::string> TempNameList;
typedef std::vector<int> TempList;
    
// Needed for GCCXML to see the types
namespace details {
inline int instantiate()
{
    int a = sizeof(boost::shared_ptr<IVehicle>);
    a += sizeof(std::vector<std::string>);
    a += sizeof(std::vector<int>);
    return a;
}
}
    
} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_COMMON_06_11_2006
