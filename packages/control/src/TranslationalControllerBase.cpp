/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/RotationalControllerBase.h
 */

// Project Includes
#include "control/include/TranslationalControllerBase.h"

namespace ram {
namespace control {
    
TranslationalControllerBase::TranslationalControllerBase(
    core::ConfigNode config) :
    m_controlMode(ControlMode::OPEN_LOOP)
{
    init(config);
}
    
math::Vector3 TranslationalControllerBase::translationalUpdate(
    double timestep,
    estimation::IStateEstimatorPtr estimator,
    control::DesiredStatePtr desiredState)
{
    return math::Vector3::ZERO;
}

void TranslationalControllerBase::setControlMode(ControlMode::ModeType mode)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_controlMode = mode;
}

ControlMode::ModeType TranslationalControllerBase::getControlMode()
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    return m_controlMode;
}

void TranslationalControllerBase::init(core::ConfigNode config)
{
}
    
} // namespace control
} // namespace ram
