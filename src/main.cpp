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
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/OrbitManipulator>
#include <osg/Texture2D>

bool loadShaderSource(osg::Shader* obj, const std::string& fileName )
{
    std::string fqFileName = osgDB::findDataFile(fileName);
    if( fqFileName.length() == 0 )
    {
        std::cout << "File \"" << fileName << "\" not found." << std::endl;
        return false;
    }
    bool success = obj->loadShaderSourceFromFile( fqFileName.c_str());
    if ( !success  )
    {
        std::cout << "Couldn't load file: " << fileName << std::endl;
        return false;
    }
    else
    {
        return true;
    }
}


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
    fog->setMode( osg::Fog::EXP2 );
    fog->setStart( 0.0f );
    fog->setEnd(board.getFieldSizeX() * 20);
    fog->setDensity(0.03);
    fog->setColor( osg::Vec4(0.2, 0.2, 0.2, 1.0) );


    root->getOrCreateStateSet()->setAttributeAndModes(fog.get());

    // Print node graph
    InfoVisitor info;
    root->accept(info);

    // Start viewer
    osgViewer::Viewer viewer;

    double height = std::min(board.getFieldSizeX(), board.getFieldSizeY()) / 1.5;

    auto fpsManipulator = make_ref<FPSManipulator>(board);
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
    viewer.setSceneData( root );

    osgViewer::Viewer::Windows windows;
    viewer.getWindows(windows);
    viewer.getCamera()->setClearColor(osg::Vec4{0, 0, 0, 0});

    viewer.getCamera()->getView()->setLightingMode(osg::View::HEADLIGHT);
    auto defaultLight = viewer.getCamera()->getView()->getLight();
    defaultLight->setDiffuse(osg::Vec4(0, 0, 0, 1));
    defaultLight->setAmbient(osg::Vec4(0, 0, 0, 1));
    defaultLight->setSpecular(osg::Vec4(0, 0, 0, 1));

    auto lightSource = make_ref<osg::LightSource>();
    lightSource->setReferenceFrame(osg::LightSource::ABSOLUTE_RF);
    auto light = lightSource->getLight();
    light->setPosition(osg::Vec4{0, 0, -1, 1});
    light->setDirection(osg::Vec3{0, 0, -1});
    light->setSpotExponent(0.05);
    light->setSpotCutoff(3.5);
    light->setDiffuse(osg::Vec4(1, 1, 1, 1));
    light->setAmbient(osg::Vec4(0.6, 0.6, 0.6, 1));
    light->setSpecular(osg::Vec4(1, 1, 1, 1));

    root->addChild(lightSource);

    auto program = make_ref<osg::Program>();
    auto fragmentObject = make_ref<osg::Shader>(osg::Shader::FRAGMENT);
    loadShaderSource(fragmentObject, dbPath + "/shader.frag");
    auto vertexObject = make_ref<osg::Shader>(osg::Shader::VERTEX);
    loadShaderSource(vertexObject, dbPath + "/shader.vert");
//    program->addShader(vertexObject);
//    program->addShader(fragmentObject);
    root->getOrCreateStateSet()->setAttributeAndModes(program, osg::StateAttribute::ON);
//    root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);


    for(osgViewer::GraphicsWindow *window: windows)
    {
        window->useCursor(false);
    }

    return viewer.run();
}
