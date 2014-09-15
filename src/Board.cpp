#include "Board.h"
#include "make_ref.h"

#include <cassert>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Fog>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

Board::Board(std::vector<std::string> &def, double sizeX, double sizeY, std::string dbPath)
    : m_sizeX(sizeX)
    , m_sizeY(sizeY)
    , m_dbPath(dbPath)
{
    assert(def.size() > 0);
    m_fieldMap.resize(def.size());

    auto height = def.size();
    auto width = def[0].size();

    for(auto elem : def) {
        assert(elem.size() >= width);
    }

    for(auto& elem : m_fieldMap) {
        elem.resize(width, FIELD_EMPTY);
    }

    for(auto i = 0; i < height; ++i)
        for(auto j = 0; j < width; ++j)
            m_fieldMap[i][j] = (def[i][j] == '*' ? FIELD_WALL : FIELD_EMPTY);
}

double Board::getSizeX() const
{
    return m_sizeX;
}

double Board::getSizeY() const
{
    return m_sizeY;
}

uint32_t Board::getFieldCountX() const
{
    return m_fieldMap[0].size();
}

uint32_t Board::getFieldCountY() const
{
    return m_fieldMap.size();
}

double Board::getFieldSizeX() const
{
    return m_sizeX / getFieldCountX();
}

double Board::getFieldSizeY() const
{
    return m_sizeY / getFieldCountY();
}

double Board::getFieldCenterX(uint32_t x) const
{
    return getFieldSizeX() / 2 + x * getFieldSizeX();
}

double Board::getFieldCenterY(uint32_t y) const
{
    return getSizeY() - getFieldSizeY() / 2 - y * getFieldSizeY();
}

Board::FieldType Board::getField(uint32_t x, uint32_t y) const
{
    return m_fieldMap[y][x];
}

void Board::setField(const double x, const double y, const FieldType fieldType)
{
    const auto intX = getFieldX(x);
    const auto intY = getFieldY(y);
    setField(intX, intY, fieldType);
}

void Board::setField(uint32_t x, uint32_t y, Board::FieldType type)
{
    m_fieldMap[y][x] = type;
}

std::vector<std::tuple<uint32_t, uint32_t>> Board::getEmptyFields() const
{
    std::vector<std::tuple<uint32_t, uint32_t>> empty;
    for(auto y = 0; y < getFieldCountY(); ++y) { // y
        for(auto x = 0; x < getFieldCountX(); ++x) { // x
            if(m_fieldMap[y][x] == FieldType::FIELD_EMPTY) {
                empty.push_back(std::tuple<uint32_t, uint32_t>{x, y});
            }
        }
    }

    return std::move(empty);
}

uint32_t Board::getFieldX(double x) const
{
    return static_cast<uint32_t>(x / getFieldSizeX());
}

uint32_t Board::getFieldY(double y) const
{
    return getFieldCountY() - (1 + static_cast<uint32_t >(y / getFieldSizeY()));
}

Board::FieldType Board::getField(const double x, const double y) const
{
    const auto intX = getFieldX(x);
    const auto intY = getFieldY(y);
    return getField(intX, intY);
}

osg::ref_ptr<osg::Node> Board::draw() const
{
    auto normalize = osg::Vec3d(0.0f, 0.0f, 0.0f);

    double blockSizeZ = std::max(getFieldSizeX(), getFieldSizeY());
    uint16_t blockCountZ = 2;

    auto overlaySize = 0;//std::min(getFieldSizeX(), getFieldSizeY()) / 100;

    auto boardObj = make_ref<osg::Group>();

    for(auto y = 0; y < getFieldCountY(); ++y) { // y
        for(auto x = 0; x < getFieldCountX(); ++x) { // x
            if(m_fieldMap[y][x] == FieldType::FIELD_WALL) {
                auto center = osg::Vec3d(getFieldCenterX(x), getFieldCenterY(y), blockSizeZ / 2) - normalize;
//                auto dr = make_ref<osg::ShapeDrawable>();
//                dr->setShape(new osg::Box(center, getFieldSizeX() + overlaySize, getFieldSizeY() + overlaySize, blockSizeZ));
//                dr->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
//
//                auto XMod = osg::Vec3d{getFieldSizeX() / 2 + overlaySize, 0, 0};
//                auto YMod = osg::Vec3d{0, getFieldSizeY() / 2 + overlaySize, 0};
//                auto ZMod = osg::Vec3d{0, 0, blockSizeZ / 2};
//
//                osg::Geode* WallGeode = new osg::Geode();
//                osg::Geometry* WallGeometry = new osg::Geometry();
//
//                WallGeode->addDrawable(WallGeometry);
//                //specify vertices
//                osg::Vec3dArray* WallVertices = new osg::Vec3dArray;
//
//
//
//                WallVertices->push_back( osg::Vec3d((center - XMod)[0], (center - YMod)[1], (center - ZMod)[2]) ); // front left
//                WallVertices->push_back( osg::Vec3d((center + XMod)[0], (center - YMod)[1], (center - ZMod)[2]) ); // front right
//                WallVertices->push_back( osg::Vec3d((center + XMod)[0], (center + YMod)[1], (center - ZMod)[2]) ); // back right
//                WallVertices->push_back( osg::Vec3d((center - XMod)[0], (center + YMod)[1], (center - ZMod)[2]) ); // back left
//                WallVertices->push_back( osg::Vec3d((center - XMod)[0], (center - YMod)[1], (center + ZMod)[2]) ); // front left
//                WallVertices->push_back( osg::Vec3d((center + XMod)[0], (center - YMod)[1], (center + ZMod)[2]) ); // front right
//                WallVertices->push_back( osg::Vec3d((center + XMod)[0], (center + YMod)[1], (center + ZMod)[2]) ); // back right
//                WallVertices->push_back( osg::Vec3d((center - XMod)[0], (center + YMod)[1], (center + ZMod)[2]) ); // back left
//                WallGeometry->setVertexArray( WallVertices );
//
//                //specify the kind of geometry we want to draw here
//                osg::DrawElementsUInt* WallBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
//                //specify the order we want to draw the base of our geometry
//                WallBase->push_back(0);
//                WallBase->push_back(1);
//                WallBase->push_back(2);
//                WallBase->push_back(3);
//                                                        // 0 3        4 7
//                WallBase->push_back(4);                 // 1 2        5 6
//                WallBase->push_back(5);
//                WallBase->push_back(6);
//                WallBase->push_back(7);
//
//                WallBase->push_back(0);
//                WallBase->push_back(1);
//                WallBase->push_back(5);
//                WallBase->push_back(4);
//
//                WallBase->push_back(2);
//                WallBase->push_back(3);
//                WallBase->push_back(7);
//                WallBase->push_back(6);
////
////
//                WallBase->push_back(0);
//                WallBase->push_back(3);
//                WallBase->push_back(7);
//                WallBase->push_back(4);
//
//
//                WallBase->push_back(1);
//                WallBase->push_back(2);
//                WallBase->push_back(6);
//                WallBase->push_back(5);
//                WallGeometry->addPrimitiveSet(WallBase);
//
//
//                osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
//
//                texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
//                texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
//                texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
//                texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
//
//
//                texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
//                texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
//                texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
//                texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
//                WallGeometry->setTexCoordArray( 0, texcoords.get() );
////
////
//                osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
//                osg::ref_ptr<osg::Image> image = osgDB::readImageFile( m_dbPath + "/wall3_small.bmp" );
//                texture->setImage( image.get() );
//
//                auto drgd = make_ref<osg::Geode>();
//
//                drgd->addDrawable(dr);
//                WallGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get() );

                osg::Geode* WallGeode = new osg::Geode();
                osg::Geometry* WallGeometry = new osg::Geometry();

                WallGeode->addDrawable(WallGeometry);
                //specify vertices
                osg::Vec3dArray* WallVertices = new osg::Vec3dArray;

                uint32_t lod = 15;
                double totalSizeX = getFieldSizeX();
                double totalSizeY = getFieldSizeY();
                double totalSizeZ = blockSizeZ;

                double sizeX = totalSizeX / lod;
                double sizeY = totalSizeY / lod;
                double sizeZ = totalSizeZ / lod;

                osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
                osg::ref_ptr<osg::Image> image = osgDB::readImageFile( m_dbPath + "/wall3_small.bmp" );
                texture->setImage( image.get() );

                for(int z = 0; z < blockCountZ; ++z) {
                    for(int i = lod - 1; i >= 0; --i) {
                        for(int j = 0; j < lod; ++j) {
                            WallVertices->push_back(osg::Vec3d(0, i * sizeY, j * sizeZ));
                            WallVertices->push_back(osg::Vec3d(0, i * sizeY + sizeY, j * sizeZ));
                            WallVertices->push_back(osg::Vec3d(0, i * sizeY + sizeY, j * sizeZ + sizeZ));
                            WallVertices->push_back(osg::Vec3d(0, i * sizeY, j * sizeZ + sizeZ));
                        }
                    }

                    for(int i = lod - 1; i >= 0; --i) {
                        for(int j = 0; j < lod; ++j) {
                            WallVertices->push_back(osg::Vec3d(totalSizeX, i * sizeY, j * sizeZ));
                            WallVertices->push_back(osg::Vec3d(totalSizeX, i * sizeY + sizeY, j * sizeZ));
                            WallVertices->push_back(osg::Vec3d(totalSizeX, i * sizeY + sizeY, j * sizeZ + sizeZ));
                            WallVertices->push_back(osg::Vec3d(totalSizeX, i * sizeY, j * sizeZ + sizeZ));
                        }
                    }

                    for(int i = 0; i < lod; ++i) {
                        for(int j = 0; j < lod; ++j) {
                            WallVertices->push_back(osg::Vec3d(i * sizeX, 0, j * sizeZ));
                            WallVertices->push_back(osg::Vec3d(i * sizeX + sizeX, 0, j * sizeZ));
                            WallVertices->push_back(osg::Vec3d(i * sizeX + sizeX, 0, j * sizeZ + sizeZ));
                            WallVertices->push_back(osg::Vec3d(i * sizeX, 0, j * sizeZ + sizeZ));
                        }
                    }

                    for(int i = 0; i < lod; ++i) {
                        for(int j = 0; j < lod; ++j) {
                            WallVertices->push_back(osg::Vec3d(i * sizeX, totalSizeY, j * sizeZ));
                            WallVertices->push_back(osg::Vec3d(i * sizeX + sizeX, totalSizeY, j * sizeZ));
                            WallVertices->push_back(osg::Vec3d(i * sizeX + sizeX, totalSizeY, j * sizeZ + sizeZ));
                            WallVertices->push_back(osg::Vec3d(i * sizeX, totalSizeY, j * sizeZ + sizeZ));
                        }
                    }

                    WallGeometry->setVertexArray( WallVertices );

                    //specify the kind of geometry we want to draw here
                    osg::DrawElementsUInt* WallBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
                    for(int i = 0; i < lod * lod * 4 * 4; ++i)
                        WallBase->push_back(i);

                    WallGeometry->addPrimitiveSet(WallBase);

                    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

                    double tsizeX = 1.0 / lod;
                    double tsizeY = 1.0 / lod;

                    for(int i = 0; i < lod; ++i) {
                        for(int j = 0; j < lod; ++j) {
                            texcoords->push_back(osg::Vec2d(i * tsizeX, j * tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX + tsizeX, j * tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX + tsizeX, j * tsizeY + tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX, j * tsizeY + tsizeY));
                        }
                    }

                    for(int i = 0; i < lod; ++i) {
                        for(int j = 0; j < lod; ++j) {
                            texcoords->push_back(osg::Vec2d(i * tsizeX, j * tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX + tsizeX, j * tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX + tsizeX, j * tsizeY + tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX, j * tsizeY + tsizeY));
                        }
                    }

                    for(int i = 0; i < lod; ++i) {
                        for(int j = 0; j < lod; ++j) {
                            texcoords->push_back(osg::Vec2d(i * tsizeX, j * tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX + tsizeX, j * tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX + tsizeX, j * tsizeY + tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX, j * tsizeY + tsizeY));
                        }
                    }

                    for(int i = 0; i < lod; ++i) {
                        for(int j = 0; j < lod; ++j) {
                            texcoords->push_back(osg::Vec2d(i * tsizeX, j * tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX + tsizeX, j * tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX + tsizeX, j * tsizeY + tsizeY));
                            texcoords->push_back(osg::Vec2d(i * tsizeX, j * tsizeY + tsizeY));
                        }
                    }

                    WallGeometry->setTexCoordArray( 0, texcoords.get() );

                    WallGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get() );

                    auto translate = make_ref<osg::MatrixTransform>(osg::Matrix::translate(getFieldCenterX(x) - getFieldSizeX() / 2, getFieldCenterY(y) - getFieldSizeY() / 2, z * blockSizeZ));

                    translate->addChild(WallGeode);

                    boardObj->addChild(translate);
                }

            }
        }
    }

    osg::Geode* FloorGeode = new osg::Geode();
    osg::Geometry* FloorGeometry = new osg::Geometry();

    FloorGeode->addDrawable(FloorGeometry);
    //specify vertices
    osg::Vec3dArray* FloorVertices = new osg::Vec3dArray;

    uint32_t FloorLOD = 1000;
    double totalSizeX = getSizeX();
    double totalSizeY = getSizeY();

    double sizeX = totalSizeX / FloorLOD;
    double sizeY = totalSizeY / FloorLOD;

    for(int i = 0; i < FloorLOD; ++i) {
        for(int j = 0; j < FloorLOD; ++j) {
            FloorVertices->push_back(osg::Vec3d(i * sizeX, j * sizeY, 0));
            FloorVertices->push_back(osg::Vec3d(i * sizeX + sizeX, j * sizeY, 0));
            FloorVertices->push_back(osg::Vec3d(i * sizeX + sizeX, j * sizeY + sizeY, 0));
            FloorVertices->push_back(osg::Vec3d(i * sizeX, j * sizeY + sizeY, 0));
        }
    }

    FloorGeometry->setVertexArray( FloorVertices );

    //specify the kind of geometry we want to draw here
    osg::DrawElementsUInt* FloorBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
    for(int i = 0; i < FloorLOD * FloorLOD * 4; ++i)
        FloorBase->push_back(i);

    FloorGeometry->addPrimitiveSet(FloorBase);

    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

    double tsizeX = 1.0;
    double tsizeY = 1.0;

    for(int i = 0; i < FloorLOD; ++i) {
        for(int j = 0; j < FloorLOD; ++j) {
            texcoords->push_back(osg::Vec2d(0, 0));
            texcoords->push_back(osg::Vec2d(tsizeX, 0));
            texcoords->push_back(osg::Vec2d(tsizeX, tsizeY));
            texcoords->push_back(osg::Vec2d(0, tsizeY));
        }
    }

    FloorGeometry->setTexCoordArray( 0, texcoords.get() );

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile( m_dbPath + "/floor.bmp" );
    texture->setImage( image.get() );

    FloorGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get() );

    osg::Geode* CeilGeode = new osg::Geode();
    osg::Geometry* CeilGeometry = new osg::Geometry();

    CeilGeode->addDrawable(CeilGeometry);
    //specify vertices
    osg::Vec3dArray* CeilVertices = new osg::Vec3dArray;

    for(int i = 0; i < FloorLOD; ++i) {
        for(int j = 0; j < FloorLOD; ++j) {
            CeilVertices->push_back(osg::Vec3d(i * sizeX, j * sizeY, 0));
            CeilVertices->push_back(osg::Vec3d(i * sizeX + sizeX, j * sizeY, 0));
            CeilVertices->push_back(osg::Vec3d(i * sizeX + sizeX, j * sizeY + sizeY, 0));
            CeilVertices->push_back(osg::Vec3d(i * sizeX, j * sizeY + sizeY, 0));
        }
    }

    CeilGeometry->setVertexArray( CeilVertices );

    //specify the kind of geometry we want to draw here
    osg::DrawElementsUInt* CeilBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
    for(int i = 0; i < FloorLOD * FloorLOD * 4; ++i)
        CeilBase->push_back(i);

    CeilGeometry->addPrimitiveSet(CeilBase);

    osg::ref_ptr<osg::Vec2Array> CeilTexcoords = new osg::Vec2Array;

    for(int i = 0; i < FloorLOD; ++i) {
        for(int j = 0; j < FloorLOD; ++j) {
            CeilTexcoords->push_back(osg::Vec2d(0, 0));
            CeilTexcoords->push_back(osg::Vec2d(tsizeX, 0));
            CeilTexcoords->push_back(osg::Vec2d(tsizeX, tsizeY));
            CeilTexcoords->push_back(osg::Vec2d(0, tsizeY));
        }
    }

    CeilGeometry->setTexCoordArray( 0, texcoords.get() );

//    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
//    osg::ref_ptr<osg::Image> image = osgDB::readImageFile( m_dbPath + "/floor.bmp" );
//    texture->setImage( image.get() );

    CeilGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get() );

    auto CeilTranslate = make_ref<osg::MatrixTransform>(osg::Matrix::translate(0, 0, blockCountZ * blockSizeZ));
    CeilTranslate->addChild(CeilGeode);

    boardObj->addChild(FloorGeode);
    boardObj->addChild(CeilTranslate);

    return boardObj;
}
