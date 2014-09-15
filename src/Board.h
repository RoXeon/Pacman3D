#ifndef board_h
#define board_h

#include <vector>
#include <string>

#include <osg/Group>

const int32_t TEXTURE_UNIT = 1;

class Board
{
public:
    enum FieldType
    {
        FIELD_EMPTY,
        FIELD_WALL,
        FIELD_NPC,
        FIELD_PC
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

    std::tuple<uint32_t, uint32_t> getPlayerPosition() const;

    FieldType getField(const double x, const double y) const;
    FieldType getField(uint32_t x, uint32_t y) const;

    void setField(const double x, const double y, const FieldType fieldType);
    void setField(uint32_t x, uint32_t y, FieldType);

    std::vector<std::tuple<uint32_t, uint32_t>> getEmptyFields() const;

    osg::ref_ptr<osg::Node> draw() const;
    osg::ref_ptr<osg::Node> DrawSquare(double RootSizeX, double RootSizeY, uint16_t FragmentCount, uint32_t LOD, std::string TextureFile) const;

private:
    double m_sizeX;
    double m_sizeY;
    std::string m_dbPath;

    std::vector<std::vector<FieldType>> m_fieldMap;
    uint32_t m_pcX;
    uint32_t m_pcY;
};

#endif // board_h
