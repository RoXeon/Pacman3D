#ifndef board_h
#define board_h

#include <vector>
#include <string>

#include <osg/Group>

class Board
{
public:
    enum FieldType
    {
        FIELD_EMPTY,
        FIELD_WALL,
        FIELD_NPC
    };

    Board(std::vector<std::string> &def, double sizeX, double sizeY, std::string dbPath);

    double getSizeX() const;
    double getSizeY() const;

    uint32_t getFieldCountX() const;
    uint32_t getFieldCountY() const;

    double getFieldSizeX() const;
    double getFieldSizeY() const;

    double getFieldCenterX(uint32_t x) const;
    double getFieldCenterY(uint32_t y) const;

    uint32_t getFieldX(double x) const;
    uint32_t getFieldY(double y) const;

    FieldType getField(uint32_t x, uint32_t y) const;
    void setField(uint32_t x, uint32_t y, FieldType);
    std::vector<std::tuple<uint32_t, uint32_t>> getEmptyFields() const;

    osg::ref_ptr<osg::Node> draw() const;

private:
    double m_sizeX;
    double m_sizeY;
    std::string m_dbPath;

    std::vector<std::vector<FieldType>> m_fieldMap;
};

#endif // board_h