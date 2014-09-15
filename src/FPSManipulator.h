#include "Board.h"

#include <osgGA/FirstPersonManipulator>

class FPSManipulator: public osgGA::FirstPersonManipulator
{
public:
    FPSManipulator(Board &b);

protected:
    bool handleKeyDown(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    bool handleKeyUp(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    bool handleMouseMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    bool performMouseDeltaMovement(const float dx, const float dy) override;
    bool handleFrame(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    void applyAnimationStep(const double currentProgress, const double previousProgress) override;
    bool handleMouseWheel(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy) override;
    osg::Vec3 newCoords(const double distanceFront, const double distanceRight);
    void allocAnimationData() override;

    void runAnimation(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);

private:
    void getDirections(osg::Vec3 &forward, osg::Vec3 &right) const;

    Board &_board;

    int _forward = 0;
    int _right = 0;
    double _forwardSpeed = 0;
    double _rightSpeed = 0;

//    double _forwardAcceleration = 0;
//    double _rightAcceleration = 0;
};
