#include "Board.h"

#include <osgGA/FirstPersonManipulator>
#include <osgViewer/Viewer>

class FPSManipulator: public osgGA::FirstPersonManipulator
{
public:
    FPSManipulator(Board &b, osgViewer::Viewer &viewer);

    class FPSAnimationData: public osgGA::StandardManipulator::AnimationData
    {
    public:
        int _forward = 0;
        int _right = 0;
    };

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
    double getBobValue(const double progress) const;

    Board &_board;
    osgViewer::Viewer &_viewer;

    double _standardHeight = 8.0;

    int _forward = 0;
    int _right = 0;
    bool _sprint = false;
};
