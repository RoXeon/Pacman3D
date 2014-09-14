#include "make_ref.h"
#include "InfoVisitor.h"
#include "board_def.h"
#include "Board.h"
#include "NPC.h"
#include "GhostFactory.h"

#include <iostream>
#include <thread>

#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>

int main(int argc, char** argv)
{
//    osg::ArgumentParser arguments( &argc, argv );
//
//    std::string test_opt;
//    arguments.read("--test_option", test_opt);

    std::srand ( unsigned ( std::time(0) ) );

    auto board = Board(boardDefinition, boardSizeX, boardSizeY);
    auto ghostFactory = GhostFactory();

    auto main_obj = make_ref<osg::Group>();
    main_obj->addChild(board.draw().get());

    auto ghostCount = 16;
    while(ghostCount--)
    {
        main_obj->addChild(ghostFactory.drawGhost(board).get());
    }

    // init rotate
    auto init_rotate = make_ref<osg::MatrixTransform>();
    init_rotate->setMatrix( osg::Matrix::rotate(osg::PI / 2, osg::Vec3(1.0f, 0.0f, 0.0f)) );

    // spin meta-group
    auto spin = make_ref<osg::MatrixTransform>();

    // presentation rotate
    auto pres_rotate = make_ref<osg::MatrixTransform>();
    pres_rotate->setMatrix( osg::Matrix::rotate(osg::PI / 32, osg::Vec3(1.0f, 0.0f, 0.0f)) );
    pres_rotate->addChild( main_obj.get() );

    // chain rotates
    init_rotate->addChild(main_obj);
    spin->addChild(pres_rotate);

    // Root group
    auto root = make_ref<osg::Group>();
    root->addChild(init_rotate);

    // Print node graph
    InfoVisitor info;
    root->accept(info);

    // Start viewer
    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );

    //auto camera = make_ref<osg::Camera()
    //viewer.setCamera(camera);
    return viewer.run();
}