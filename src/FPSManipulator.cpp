#include "FPSManipulator.h"

#include "make_ref.h"

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

constexpr double MAX_STEP_DISTANCE = 20;
constexpr double STEP_TIME = 1.;
constexpr double DECELERATION = 5;
constexpr double ACCELERATION = 1.5;


FPSManipulator::FPSManipulator()
{
    setAllowThrow(false);
}

bool FPSManipulator::handleKeyDown(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    switch(ea.getKey())
    {
        case osgGA::GUIEventAdapter::KEY_W:
            _forward = 1;
            runAnimation(ea, us);
            return true;
        case osgGA::GUIEventAdapter::KEY_S:
            _forward = -1;
            runAnimation(ea, us);
            return true;
        case osgGA::GUIEventAdapter::KEY_A:
            _right = -1;
            runAnimation(ea, us);
            return true;
        case osgGA::GUIEventAdapter::KEY_D:
            _right = 1;
            runAnimation(ea, us);
            return true;
        case osgGA::GUIEventAdapter::KEY_Space:
            home(ea, us);
            return true;
    }

    return false;
}

bool FPSManipulator::handleKeyUp(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    switch(ea.getKey())
    {
        case osgGA::GUIEventAdapter::KEY_W:
        case osgGA::GUIEventAdapter::KEY_S:
            _forward = 0;
            return true;
        case osgGA::GUIEventAdapter::KEY_A:
        case osgGA::GUIEventAdapter::KEY_D:
            _right = 0;
            return true;
    }

    return false;
}

bool FPSManipulator::handleMouseMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    return handleMouseDeltaMovement(ea, us);
}

bool FPSManipulator::performMouseDeltaMovement(const float dx, const float dy)
{
    return osgGA::FirstPersonManipulator::performMouseDeltaMovement(dx/10000, dy/10000);
}

bool FPSManipulator::handleFrame(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    if(_right || _forward)
        runAnimation(ea, us);

    return osgGA::FirstPersonManipulator::handleFrame(ea, us);
}

void FPSManipulator::applyAnimationStep(const double currentProgress, const double previousProgress)
{
    const auto fraction = (currentProgress - previousProgress);

//    _forwardAcceleration += _forward ? fraction * ACCELERATION : fraction * DECELERATION;
//    _rightAcceleration += _right ? fraction * ACCELERATION : fraction * DECELERATION;

//    _forwardAcceleration = std::max(std::min(MAX_STEP_DISTANCE, _forwardAcceleration), 0);
//    _rightAcceleration = std::max(std::min(MAX_STEP_DISTANCE, _rightAcceleration), 0);

    const auto forwardSpeed = _forward * MAX_STEP_DISTANCE;
    const auto rightSpeed = _right * MAX_STEP_DISTANCE;
    const auto totalSpeed = std::abs(forwardSpeed) + std::abs(rightSpeed);
    if(totalSpeed == 0)
        return;

    const auto normalizedForwardSpeed = forwardSpeed * std::abs(forwardSpeed) / totalSpeed;
    const auto normalizedRightSpeed = rightSpeed * std::abs(rightSpeed) / totalSpeed;

    const auto moveForwardBy = (currentProgress - previousProgress) * normalizedForwardSpeed;
    const auto moveRightBy = (currentProgress - previousProgress) * normalizedRightSpeed;

    moveForward(moveForwardBy);
    moveRight(moveRightBy);
}

bool FPSManipulator::handleMouseWheel(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    return false;
}

bool FPSManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    return false;
}

void FPSManipulator::moveForward(const double distance)
{
    osg::Vec3 forward = _rotation * osg::Vec3{0, 0, -1};
    forward[2] = 0;
    forward.normalize();

    _eye += forward * distance;
}

void FPSManipulator::moveRight(const double distance)
{
    osg::Vec3 right = _rotation * osg::Vec3{1, 0, 0};
    right[2] = 0;
    right.normalize();

    _eye += right * distance;
}

void FPSManipulator::allocAnimationData()
{
    _animationData = make_ref<osgGA::StandardManipulator::AnimationData>();
}

void FPSManipulator::runAnimation(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    if(!isAnimating()) {
        setAnimationTime(STEP_TIME);
        _animationData->start(ea.getTime());
        us.requestRedraw();
        us.requestContinuousUpdate(true);
    }
}
