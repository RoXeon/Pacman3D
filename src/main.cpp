#include "make_ref.h"

#include <string>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgViewer/Viewer>

int main(int argc, char** argv)
{
    osg::ArgumentParser arguments( &argc, argv );

    std::string testOpt;
    arguments.read("--test_option", testOpt);

    auto shape1 = make_ref<osg::ShapeDrawable>();
    shape1->setShape( new osg::Box(osg::Vec3(-10.0f, 0.0f, 0.0f),
            1.0f, 8.0f, 50.0f) );

    auto shape2 = make_ref<osg::ShapeDrawable>();
    shape2->setShape( new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f),
            1.0f, 8.0f, 50.0f) );
    shape2->setColor( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );


    osg::ref_ptr<osg::Geode> root = new osg::Geode;
    root->addDrawable( shape1.get() );
    root->addDrawable( shape2.get() );
    //root->addDrawable( shape3.get() );


    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
    return viewer.run();
}