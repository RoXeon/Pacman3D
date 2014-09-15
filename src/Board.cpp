#include "Board.h"
#include "make_ref.h"

#include <cassert>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Fog>
#include <osg/Geometry>
#include <osg/ShadeModel>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

osg::ref_ptr<osg::Node> Board::DrawSquare(double RootSizeX, double RootSizeY, uint16_t FragmentCount, uint32_t LOD, std::string TextureFile) const
{
    osg::Geode* CeilGeode = new osg::Geode();
    osg::Geometry* CeilGeometry = new osg::Geometry();

    CeilGeode->addDrawable(CeilGeometry);
    //specify vertices
    osg::Vec3dArray* CeilVertices = new osg::Vec3dArray;
    osg::DrawElementsUInt* CeilBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
    osg::ref_ptr<osg::Vec2Array> CeilTexcoords = new osg::Vec2Array;

    double FragmentSizeX = RootSizeX / FragmentCount;
    double FragmentSizeY = RootSizeY / FragmentCount;

    double sTexSizeX = 1.0 / LOD;
    double sTexSizeY = 1.0 / LOD;

    osg::ref_ptr<osg::Texture2D> CeilTexture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> CeilImage = osgDB::readImageFile( m_dbPath + "/" + TextureFile );
    CeilTexture->setImage( CeilImage.get() );

    for(int fi = 0; fi < FragmentCount; ++fi) {
        for(int fj = 0; fj < FragmentCount; ++fj) {

            double sizeX = FragmentSizeX / LOD;
            double sizeY = FragmentSizeY / LOD;

            for(int i = 0; i < LOD; ++i) {
                for(int j = 0; j < LOD; ++j) {
                    CeilVertices->push_back(osg::Vec3d(fi * FragmentSizeX + i * sizeX, fj * FragmentSizeY + j * sizeY, 0));
                    CeilVertices->push_back(osg::Vec3d(fi * FragmentSizeX + i * sizeX + sizeX, fj * FragmentSizeY + j * sizeY, 0));
                    CeilVertices->push_back(osg::Vec3d(fi * FragmentSizeX + i * sizeX + sizeX, fj * FragmentSizeY + j * sizeY + sizeY, 0));
                    CeilVertices->push_back(osg::Vec3d(fi * FragmentSizeX + i * sizeX, fj * FragmentSizeY + j * sizeY + sizeY, 0));
                }
            }

            for(int i = 0; i < LOD; ++i) {
                for(int j = 0; j < LOD; ++j) {
                    CeilTexcoords->push_back(osg::Vec2d(i * sTexSizeX, j * sTexSizeY));
                    CeilTexcoords->push_back(osg::Vec2d(i * sTexSizeX + sTexSizeX, j * sTexSizeY));
                    CeilTexcoords->push_back(osg::Vec2d(i * sTexSizeX + sTexSizeX, j * sTexSizeY + sTexSizeY));
                    CeilTexcoords->push_back(osg::Vec2d(i * sTexSizeX, j * sTexSizeY + sTexSizeY));
                }
            }
        }
    }

    uint64_t ElemCount = LOD * LOD * 4 * FragmentCount * FragmentCount;
    for(uint64_t i = 0; i < ElemCount; ++i)
        CeilBase->push_back(i);

    CeilGeometry->setVertexArray( CeilVertices );
    CeilGeometry->addPrimitiveSet(CeilBase);

    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    normals->push_back( osg::Vec3(0.0f, 0.0f, 1.0f) );

    CeilGeometry->setNormalArray( normals );
    CeilGeometry->setNormalBinding( osg::Geometry::BIND_OVERALL );
    CeilGeometry->setTexCoordArray(1, CeilTexcoords.get() );


    osg::ref_ptr<osg::Material> material = new osg::Material;
    material->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(0.5f, 0.5f, 0.5f, 0.5f) );
    material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 0.5f) );
    material->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 0.5f) );
    material->setDataVariance(osg::Material::STATIC);
    material->setShininess(osg::Material::FRONT_AND_BACK, 0.2);

    //CeilGeode->getOrCreateStateSet()->setAttributeAndModes(material);
    CeilGeode->getOrCreateStateSet()->setTextureAttributeAndModes(TEXTURE_UNIT, CeilTexture.get() );

//    osg::TexEnv* blendTexEnv = new osg::TexEnv;
//    blendTexEnv->setMode(osg::TexEnv::BLEND);
//    CeilGeode->getOrCreateStateSet()->setTextureAttribute(1, blendTexEnv);
//
//    osg::TexEnv* decalTexEnv = new osg::TexEnv;
//    decalTexEnv->setMode(osg::TexEnv::DECAL);
//    CeilGeode->getOrCreateStateSet()->setTextureAttribute(2, decalTexEnv);

    return osg::ref_ptr<osg::Node>(CeilGeode);
}


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

    auto boardObj = make_ref<osg::Group>();

    for(auto y = 0; y < getFieldCountY(); ++y) { // y
        for(auto x = 0; x < getFieldCountX(); ++x) { // x
            if(m_fieldMap[y][x] == FieldType::FIELD_WALL) {
 //           if(x == 15 && y == 14) {
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

                    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
                    for(int i = 0; i < lod; ++i) {
                        for(int j = 0; j < lod; ++j) {
                            normals->push_back( osg::Vec3(-1.0f, 0.0f, 0.0f) );
                            normals->push_back( osg::Vec3(-1.0f, 0.0f, 0.0f) );
                            normals->push_back( osg::Vec3(-1.0f, 0.0f, 0.0f) );
                            normals->push_back( osg::Vec3(-1.0f, 0.0f, 0.0f) );
                        }
                    }

                    for(int i = 0; i < lod; ++i) {
                        for(int j = 0; j < lod; ++j) {
                            normals->push_back( osg::Vec3(1.0f, 0.0f, 0.0f) );
                            normals->push_back( osg::Vec3(1.0f, 0.0f, 0.0f) );
                            normals->push_back( osg::Vec3(1.0f, 0.0f, 0.0f) );
                            normals->push_back( osg::Vec3(1.0f, 0.0f, 0.0f) );
                        }
                    }

                    for(int i = 0; i < lod; ++i) {
                        for(int j = 0; j < lod; ++j) {
                            normals->push_back( osg::Vec3(0.0f, -1.0f, 0.0f) );
                            normals->push_back( osg::Vec3(0.0f, -1.0f, 0.0f) );
                            normals->push_back( osg::Vec3(0.0f, -1.0f, 0.0f) );
                            normals->push_back( osg::Vec3(0.0f, -1.0f, 0.0f) );
                        }
                    }

                    for(int i = 0; i < lod; ++i) {
                        for(int j = 0; j < lod; ++j) {
                            normals->push_back( osg::Vec3(0.0f, 1.0f, 0.0f) );
                            normals->push_back( osg::Vec3(0.0f, 1.0f, 0.0f) );
                            normals->push_back( osg::Vec3(0.0f, 1.0f, 0.0f) );
                            normals->push_back( osg::Vec3(0.0f, 1.0f, 0.0f) );
                        }
                    }

                    for(int i = 0; i < lod; ++i) {
                        for(int j = 0; j < lod; ++j) {
                            normals->push_back( osg::Vec3(0.0f, -1.0f, 0.0f) );
                            normals->push_back( osg::Vec3(0.0f, -1.0f, 0.0f) );
                            normals->push_back( osg::Vec3(0.0f, -1.0f, 0.0f) );
                            normals->push_back( osg::Vec3(0.0f, -1.0f, 0.0f) );
                        }
                    }

                    WallGeometry->setTexCoordArray(1, texcoords.get() );
                    WallGeode->getOrCreateStateSet()->setTextureAttributeAndModes(1, texture.get() );

                    WallGeometry->setNormalArray(normals);
                    WallGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

                    osg::ref_ptr<osg::Material> material = new osg::Material;
                    material->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f) );
                    material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f) );
                    material->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );
                    material->setDataVariance(osg::Material::STATIC);
                    material->setShininess(osg::Material::FRONT_AND_BACK, 1.0);

                    //WallGeode->getOrCreateStateSet()->setAttributeAndModes(material);

                    auto translate = make_ref<osg::MatrixTransform>(osg::Matrix::translate(getFieldCenterX(x) - getFieldSizeX() / 2, getFieldCenterY(y) - getFieldSizeY() / 2, z * blockSizeZ));
                    translate->addChild(WallGeode);

                    boardObj->addChild(translate);
                }

            }
        }
    }

    auto FloorGeode = DrawSquare(getSizeX(), getSizeY(), 8, 100, "floor.bmp");
    auto CeilGeode = DrawSquare(getSizeX(), getSizeY(), 8, 100, "ceil.bmp");
    auto CeilTranslate = make_ref<osg::MatrixTransform>(osg::Matrix::translate(0, 0, blockCountZ * blockSizeZ));
    CeilTranslate->addChild(CeilGeode);

    boardObj->addChild(FloorGeode);
    boardObj->addChild(CeilTranslate);

    return boardObj;
}
