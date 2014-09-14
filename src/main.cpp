#include "make_ref.h"
#include "InfoVisitor.h"
#include "board_def.h"
#include "Board.h"
#include "NPC.h"
#include "GhostFactory.h"
#include "FPSManipulator.h"

#include <iostream>
#include <thread>

#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osg/Fog>
#include <osgDB/ReadFile>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/OrbitManipulator>
#include <osg/Texture2d>

int main(int argc, char** argv)
{
    osg::ArgumentParser arguments( &argc, argv );

    std::string dbPath;
    arguments.read("--db_path", dbPath);

    std::srand ( unsigned ( std::time(0) ) );

    auto board = Board(boardDefinition, boardSizeX, boardSizeY, dbPath);
    auto ghostFactory = GhostFactory();

    auto main_obj = make_ref<osg::Group>();
    main_obj->addChild(board.draw().get());

    auto ghostModel = osgDB::readNodeFile(dbPath + "/cow.osg");
    auto ghostCount = 16;
    while(ghostCount--)
    {
        main_obj->addChild(ghostFactory.drawGhost(board, ghostModel).get());
    }

    // init rotate
    auto init_rotate = make_ref<osg::MatrixTransform>();
    init_rotate->setMatrix( osg::Matrix::rotate(osg::PI * 2, osg::Vec3(1.0f, 0.0f, 0.0f)) );

    // chain rotates
    init_rotate->addChild(main_obj);

    // Root group
    auto root = make_ref<osg::Group>();
    root->addChild(init_rotate);

    // Setup fog
    osg::ref_ptr<osg::Fog> fog = new osg::Fog;
    fog->setMode( osg::Fog::EXP );
    fog->setStart( 0.0f );
    fog->setEnd(board.getFieldSizeX() * 20);
    fog->setDensity(0.03);
    fog->setColor( osg::Vec4(0.5, 0.5, 0.5, 1.0) );

    //root->getOrCreateStateSet()->setAttributeAndModes(fog.get());

    // Print node graph
    InfoVisitor info;
    root->accept(info);

    // Start viewer
    osgViewer::Viewer viewer;

    double height = std::min(board.getFieldSizeX(), board.getFieldSizeY()) / 1.5;

    auto fpsManipulator = make_ref<FPSManipulator>();
    fpsManipulator->setHomePosition(
        osg::Vec3d(board.getFieldCenterX(1), board.getFieldCenterY(10), height),
        osg::Vec3d(0.0f, 0.0f, height),
        osg::Vec3d(0.0f, 0.0f, 1.0f)
    );


    auto keySwitch = make_ref<osgGA::KeySwitchMatrixManipulator>();
    keySwitch->addNumberedMatrixManipulator(make_ref<osgGA::OrbitManipulator>());
    keySwitch->addNumberedMatrixManipulator(fpsManipulator);
    viewer.setCameraManipulator(keySwitch);

    viewer.home();


//    osg::Geode* WallGeode = new osg::Geode();
//    osg::Geometry* WallGeometry = new osg::Geometry();
//
//    WallGeode->addDrawable(WallGeometry);
//    //specify vertices
//    osg::Vec3dArray* WallVertices = new osg::Vec3dArray;
//    WallVertices->push_back( osg::Vec3d(0,0,0)); // front left
//    WallVertices->push_back( osg::Vec3d(16,0,0) ); // front right
//    WallVertices->push_back( osg::Vec3d(16,16,0) ); // back right
//    WallVertices->push_back( osg::Vec3d(0,16,0) ); // back left
//    WallVertices->push_back( osg::Vec3d(0,0,16)); // front left
//    WallVertices->push_back( osg::Vec3d(16,0,16) ); // front right
//    WallVertices->push_back( osg::Vec3d(16,16,16) ); // back right
//    WallVertices->push_back( osg::Vec3d(0,16,16) ); // back left
//    WallGeometry->setVertexArray( WallVertices );
//
//    //specify the kind of geometry we want to draw here
//    osg::DrawElementsUInt* WallBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
//    //specify the order we want to draw the base of our geometry
//    WallBase->push_back(0);
//    WallBase->push_back(1);
//    WallBase->push_back(2);
//    WallBase->push_back(3);
//    // 0 3        4 7
//    WallBase->push_back(4);                 // 1 2        5 6
//    WallBase->push_back(5);
//    WallBase->push_back(6);
//    WallBase->push_back(7);
//
//    WallBase->push_back(0);
//    WallBase->push_back(1);
//    WallBase->push_back(5);
//    WallBase->push_back(4);
//
//    WallBase->push_back(2);
//    WallBase->push_back(3);
//    WallBase->push_back(7);
//    WallBase->push_back(6);
////
////
//    WallBase->push_back(0);
//    WallBase->push_back(3);
//    WallBase->push_back(7);
//    WallBase->push_back(4);
//
//
//    WallBase->push_back(1);
//    WallBase->push_back(2);
//    WallBase->push_back(6);
//    WallBase->push_back(5);
//    WallGeometry->addPrimitiveSet(WallBase);
//
//
//    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
//
//    texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
//    texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
//    texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
//    texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
//
//
//    texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
//    texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
//    texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
//    texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
//    WallGeometry->setTexCoordArray( 0, texcoords.get() );
////
////
//    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
//    osg::ref_ptr<osg::Image> image = osgDB::readImageFile( "/Users/RoXeon/Projects/Pacman3D/db/wall3_small.bmp" );
//    texture->setImage( image.get() );
//
//    WallGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get() );

    viewer.setSceneData( root.get() );

    osgViewer::Viewer::Windows windows;
    viewer.getWindows(windows);
    viewer.getCamera()->setClearColor(osg::Vec4{0, 0, 0, 0});

    viewer.getCamera()->getView()->setLightingMode(osg::View::HEADLIGHT);
    auto defaultLight = viewer.getCamera()->getView()->getLight();
    defaultLight->setDiffuse(osg::Vec4(1, 1, 1, 1));
    defaultLight->setAmbient(osg::Vec4(1, 1, 1, 1));
    defaultLight->setSpecular(osg::Vec4(1, 1, 1, 1));

    for(osgViewer::GraphicsWindow *window: windows)
    {
        window->useCursor(false);
    }

    return viewer.run();
}
