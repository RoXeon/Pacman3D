#ifndef board_h
#define board_h

#include <vector>
#include <string>

#include <osg/Group>

class Board
{
public:
    Board(std::vector<std::string>& def, double sizeX, double sizeY);

    double getSizeX() const;
    double getSizeY() const;

    uint32_t getFieldCountX() const;
    uint32_t getFieldCountY() const;

    double getFieldSizeX() const;
    double getFieldSizeY() const;

    double getFieldCenterX(uint32_t x) const;
    double getFieldCenterY(uint32_t y) const;


    osg::ref_ptr<osg::Group> draw() const;

private:
    double m_sizeX;
    double m_sizeY;

    std::vector<std::vector<bool>> m_fieldMap;
};

#endif // board_h