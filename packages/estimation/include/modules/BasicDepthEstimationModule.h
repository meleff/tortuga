/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/modules/BasicDepthEstimationModule.h
 */

/* This is currently a template for a Depth EstimationModule implementation */


#ifndef RAM_ESTIMATION_BASICDEPTHESTIMATIONMODULE_H
#define RAM_ESTIMATION_BASICDEPTHESTIMATIONMODULE_H

// STD Includes

// Library Includes

// Project Includes
#include "estimation/include/EstimatedState.h"
#include "estimation/include/EstimationModule.h"

#include "core/include/ConfigNode.h"
#include "core/include/Event.h"

namespace ram {
namespace estimation {

class BasicDepthEstimationModule : public EstimationModule
{
public:
    BasicDepthEstimationModule(core::ConfigNode config,
                               core::EventHubPtr eventHub);
    ~BasicDepthEstimationModule(){};

    /* The Depth Estimation routine goes here.  It should store the new estimated
       state in estimatedState. */
    virtual void update(core::EventPtr event, EstimatedStatePtr estimatedState);

private:
    /* any necessary persistent variables should be declared here */
    std::string m_name;
};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_BASICDEPTHESTIMATIONMODULE_H
