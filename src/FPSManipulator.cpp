#include "FPSManipulator.h"

#include "make_ref.h"

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

FPSManipulator::FPSManipulator()
{
}

bool FPSManipulator::handleKeyDown(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    switch(ea.getKey())
    {
        case osgGA::GUIEventAdapter::KEY_W:
            _forwardSpeed = 1;
            break;
        case osgGA::GUIEventAdapter::KEY_S:
            _forwardSpeed = -1;
            break;
        case osgGA::GUIEventAdapter::KEY_A:
            _rightSpeed = -1;
            break;
        case osgGA::GUIEventAdapter::KEY_D:
            _rightSpeed = 1;
            break;
        case osgGA::GUIEventAdapter::KEY_Space:
            home(ea, us);
    }

    move();
    return true;
}

bool FPSManipulator::handleKeyUp(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    switch(ea.getKey())
    {
        case osgGA::GUIEventAdapter::KEY_W:
            _forwardSpeed = 0;
            break;
        case osgGA::GUIEventAdapter::KEY_S:
            _forwardSpeed = 0;
            break;
        case osgGA::GUIEventAdapter::KEY_A:
            _rightSpeed = 0;
            break;
        case osgGA::GUIEventAdapter::KEY_D:
            _rightSpeed = 0;
            break;
    }

    move();
    return true;
}

bool FPSManipulator::handleMouseMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    return handleMouseDeltaMovement(ea, us);
}

bool FPSManipulator::performMouseDeltaMovement(const float dx, const float dy)
{
    return osgGA::FirstPersonManipulator::performMouseDeltaMovement(dx/10000, dy/10000);
}

void FPSManipulator::move()
{
    auto forward = _rotation * osg::Vec3{0, 0, -1};
    forward[2] = 0;
    forward.normalize();

    auto right = _rotation * osg::Vec3{1, 0, 0};
    right[2] = 0;
    right.normalize();

    _eye += forward * _forwardSpeed + right * _rightSpeed;
}
