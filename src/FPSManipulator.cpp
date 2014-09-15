#include "FPSManipulator.h"

#include "make_ref.h"

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

constexpr double MAX_STEP_DISTANCE = 20;
constexpr double STEP_TIME = 1.;
constexpr double DECELERATION = 5;
constexpr double ACCELERATION = 1.5;


FPSManipulator::FPSManipulator(Board &b)
    : _board(b)
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
            _board.setField(_eye.x(), _eye.y(), Board::FIELD_PC);
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

    auto newEye = newCoords(moveForwardBy, moveRightBy);

    auto consideredX = newEye.x() + (newEye.x() - _eye.x() > 0 ? +1 : -1) * _board.getFieldSizeX() / 20;
    auto consideredY = newEye.y() + (newEye.y() - _eye.y() > 0 ? +1 : -1) * _board.getFieldSizeY() / 20;

    const auto fieldType = _board.getField(consideredX, consideredY);
    if(fieldType == Board::FIELD_EMPTY || fieldType == Board::FIELD_PC)
    {
        _board.setField(_eye.x(), _eye.y(), Board::FIELD_EMPTY);
        _eye = newEye;
        _board.setField(_eye.x(), _eye.y(), Board::FIELD_PC);
    }
}

bool FPSManipulator::handleMouseWheel(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    return false;
}

bool FPSManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    return false;
}

osg::Vec3 FPSManipulator::newCoords(const double distanceForward, const double distanceRight)
{
    osg::Vec3 forward = _rotation * osg::Vec3{0, 0, -1};
    forward[2] = 0;
    forward.normalize();

    osg::Vec3 right = _rotation * osg::Vec3{1, 0, 0};
    right[2] = 0;
    right.normalize();

    return _eye + forward * distanceForward + right * distanceRight;
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
