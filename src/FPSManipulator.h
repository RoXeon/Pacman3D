#include <osgGA/FirstPersonManipulator>

class FPSManipulator: public osgGA::FirstPersonManipulator
{
public:
    FPSManipulator();

protected:
    bool handleKeyDown(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    bool handleKeyUp(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    bool handleMouseMove(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us) override;
    bool performMouseDeltaMovement(const float dx, const float dy) override;

private:
    void move();

    double _forwardSpeed = 0;
    double _rightSpeed = 0;
};
