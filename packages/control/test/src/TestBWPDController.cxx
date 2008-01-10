/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee
 * All rights reserved.
 *
 * Authory: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/TestControlFunctions.cxx
 */

// Library Includes
#include <iostream>
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "math/test/include/MathChecks.h"
#include "math/include/Events.h"
#include "vehicle/test/include/MockVehicle.h"
#include "control/include/BWPDController.h"
//#include "control/test/include/ControllerTests.h"

using namespace ram;

struct Fixture
{
    Fixture() : vehicle(new MockVehicle()),
                controller(ram::vehicle::IVehiclePtr(vehicle),
                           core::ConfigNode::fromString(
                               "{ 'name' : 'TestController',"
                        	   "'angularPGain' : 10,"
                               "'angularDGain' : 1,"
                               "'desiredQuaternion' : [0, 0, 0, 1] }"))
    {}

    MockVehicle* vehicle;
    control::BWPDController controller;
};

TEST_FIXTURE(Fixture, YawControl)
{
    // First is north along horizontal (desired)
    // Rotated 32 degrees from north to the west in horizontal (actual)
    vehicle->orientation = math::Quaternion(0, 0, 0.2756, 0.9613);
    
    math::Vector3 exp_rotTorque(0, 0, -3.5497);
    controller.update(1);
    CHECK_CLOSE(exp_rotTorque, vehicle->torque, 0.0001);
}

TEST_FIXTURE(Fixture, PitchControl)
{
    math::Vector3 exp_rotTorque(0, -2.7872, 0);
    vehicle->orientation = math::Quaternion(0, 0.2164, 0, 0.9763);

    controller.update(1);
    CHECK_CLOSE(exp_rotTorque, vehicle->torque, 0.0001);
}

TEST_FIXTURE(Fixture, RollControl)
{
    math::Vector3 exp_rotTorque(0.7692, 0, 0);
    vehicle->orientation = math::Quaternion(-0.3827, 0, 0, 0.9239);

    controller.update(1);
    CHECK_CLOSE(exp_rotTorque, vehicle->torque, 0.0001);
}

TEST_FIXTURE(Fixture, DepthControl)
{
    math::Vector3 exp_tranForce(0, 0, 0);

    // Test at correct depth response
    controller.setDepth(0);
    vehicle->depth = 0;
    controller.update(1);
    CHECK_CLOSE(exp_tranForce, vehicle->force, 0.0001);

    controller.setDepth(3);
    vehicle->depth = 3;    
    controller.update(1);
    CHECK_CLOSE(exp_tranForce, vehicle->force, 0.0001);

    // Test we need to dive response
    exp_tranForce = math::Vector3(0,0,-5);
    
    controller.setDepth(5);
    vehicle->depth = 0;
    controller.update(1);
    CHECK_CLOSE(exp_tranForce, vehicle->force, 0.0001);
}

TEST_FIXTURE(Fixture, atDepth)
{
    // This assumes the default threshold for depth is 0.5
    vehicle->depth = 4;
    controller.update(1);
    
    controller.setDepth(5);
    CHECK_EQUAL(false, controller.atDepth());

    controller.setDepth(3);
    CHECK_EQUAL(false, controller.atDepth());

    controller.setDepth(4.3);
    CHECK_EQUAL(true, controller.atDepth());

    controller.setDepth(3.7);
    CHECK_EQUAL(true, controller.atDepth());
    
    controller.setDepth(4);
    CHECK(controller.atDepth());
}

void depthHelper(double* result, ram::core::EventPtr event)
{
    ram::math::NumericEventPtr nevent =
        boost::dynamic_pointer_cast<ram::math::NumericEvent>(event);
    *result = nevent->number;
}

TEST_FIXTURE(Fixture, Event_DESIRED_DEPTH_UPDATE)
{
    double actualDesiredDepth = 0;

    // Subscribe to the event
    controller.subscribe(ram::control::IController::DESIRED_DEPTH_UPDATE,
                         boost::bind(depthHelper, &actualDesiredDepth, _1));
    
    // Test at correct depth response
    controller.setDepth(5);

    CHECK_EQUAL(5, actualDesiredDepth);
}

TEST_FIXTURE(Fixture, Event_AT_DEPTH)
{
    double actualDepth = 0;

    // Subscribe to the event
    controller.subscribe(ram::control::IController::AT_DEPTH,
                         boost::bind(depthHelper, &actualDepth, _1));

    // Default Depth Threshold is 0.5
    vehicle->depth = 4;
    controller.update(1);
    controller.setDepth(3.7);
    CHECK_EQUAL(true, controller.atDepth());
    
    // Make sure the event doesn't go off when we are at depth
    controller.setDepth(5);
    controller.update(1);
    CHECK_EQUAL(0, actualDepth);

    // Ensure it does go off
    controller.setDepth(4.3);
    controller.update(1);
    CHECK_EQUAL(4, actualDepth);

    // Make sure it doesn't go off again until we have left the depth range
    actualDepth = 0;
    controller.update(1);
    CHECK_EQUAL(0, actualDepth);

    // Make sure once we leave, and come back it will go off again
    vehicle->depth = 6;
    controller.update(1);
    CHECK_EQUAL(0, actualDepth);

    vehicle->depth = 4;
    controller.setDepth(3.7);
    controller.update(1);
    CHECK_EQUAL(4, actualDepth);
}
