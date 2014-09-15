#include "FPSManipulator.h"

#include "make_ref.h"

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

namespace {
constexpr double FORWARD_STEP_DISTANCE = 10;
constexpr double FORWARD_SPRINT_DISTANCE = 15;
constexpr double RIGHT_STEP_DISTANCE = 5;
constexpr double STEP_TIME = .75;
constexpr double BOBBING_HEIGHT = 0.3;
constexpr double COLLISION_FIELD_PADDING = 0.1;
constexpr double BIDIRECTIONAL_MOVEMENT_NORMALIZATION = 0.7;

template<typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }
}

FPSManipulator::FPSManipulator(Board &b, osgViewer::Viewer &viewer, osg::Light &flashlight)
    : _board(b)
    , _viewer(viewer)
    , _flashlight(flashlight)
{
    setAllowThrow(false);
}

bool FPSManipulator::handleKeyDown(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    _sprint = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT;

    switch(ea.getUnmodifiedKey())
    {
        case osgGA::GUIEventAdapter::KEY_W: _forward =  1; return true;
        case osgGA::GUIEventAdapter::KEY_S: _forward = -1; return true;
        case osgGA::GUIEventAdapter::KEY_A: _right   = -1; return true;
        case osgGA::GUIEventAdapter::KEY_D: _right   =  1; return true;
        case osgGA::GUIEventAdapter::KEY_F: toggleFlashlight(); return true;
        case osgGA::GUIEventAdapter::KEY_Space:
            home(ea, us);
            _standardHeight = _eye.z();
            _board.setField(_eye.x(), _eye.y(), Board::FIELD_PC);
            return true;
    }

    return false;
}

bool FPSManipulator::handleKeyUp(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
    switch(ea.getUnmodifiedKey())
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
    osgViewer::Viewer::Windows windows;
    _viewer.getWindows(windows);
    for(osgViewer::GraphicsWindow *window: windows)
    {
        window->useCursor(false);
    }

    if(_right || _forward) {
        runAnimation(ea, us);

        // Possibly change step direction
        const auto ad = osg::static_pointer_cast<FPSAnimationData>(_animationData);
        ad->_right = _right;
        ad->_forward = _forward;
    }

    return osgGA::FirstPersonManipulator::handleFrame(ea, us);
}

void FPSManipulator::applyAnimationStep(const double currentProgress, const double previousProgress)
{
    const auto ad = osg::static_pointer_cast<FPSAnimationData>(_animationData);
    const auto forwardSpeed = ad->_forward * (_sprint ? FORWARD_SPRINT_DISTANCE : FORWARD_STEP_DISTANCE);
    const auto rightSpeed = ad->_right * RIGHT_STEP_DISTANCE;

    const auto normalize = ad->_forward && ad->_right;
    const auto normalizedForwardSpeed = normalize ? forwardSpeed * BIDIRECTIONAL_MOVEMENT_NORMALIZATION : forwardSpeed;
    const auto normalizedRightSpeed = normalize ? rightSpeed * BIDIRECTIONAL_MOVEMENT_NORMALIZATION : rightSpeed;

    const auto fraction = currentProgress - previousProgress;
    const auto moveForwardBy = fraction * normalizedForwardSpeed;
    const auto moveRightBy = fraction * normalizedRightSpeed;

    const auto newEye = newCoords(moveForwardBy, moveRightBy);
    const auto newHeight = _standardHeight - getBobValue(currentProgress);

    const auto paddingX = sgn(newEye.x() - _eye.x()) * _board.getFieldSizeX() * COLLISION_FIELD_PADDING;
    const auto paddingY = sgn(newEye.y() - _eye.y()) * _board.getFieldSizeY() * COLLISION_FIELD_PADDING;

    const auto fieldType = _board.getField(newEye.x() + paddingX, newEye.y() + paddingY);
    if(fieldType == Board::FIELD_EMPTY || fieldType == Board::FIELD_PC)
    {
        _board.setField(_eye.x(), _eye.y(), Board::FIELD_EMPTY);
        _eye = newEye;
        _eye[2] = newHeight;
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
    _animationData = make_ref<FPSAnimationData>();
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

void FPSManipulator::toggleFlashlight()
{
    if(_flashlight.getDiffuse() == osg::Vec4{})
    {
        _flashlight.setDiffuse(osg::Vec4(1, 1, 1, 1));
        _flashlight.setAmbient(osg::Vec4(0.6, 0.6, 0.6, 1));
        _flashlight.setSpecular(osg::Vec4(1, 1, 1, 1));
    }
    else
    {
        _flashlight.setDiffuse(osg::Vec4{});
        _flashlight.setAmbient(osg::Vec4{});
        _flashlight.setSpecular(osg::Vec4{});
    }
}

double FPSManipulator::getBobValue(const double progress) const
{
    if(progress == 1)
        return 0;

    if(progress < 0.5)
        return BOBBING_HEIGHT * progress * 2;

    return BOBBING_HEIGHT * (1.0 - progress) * 2;
}
