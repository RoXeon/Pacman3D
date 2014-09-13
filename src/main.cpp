#include "make_ref.h"
#include "InfoVisitor.h"

#include <string>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osg/AnimationPath>

int main(int argc, char** argv)
{
    osg::ArgumentParser arguments( &argc, argv );

    std::string test_opt;
    arguments.read("--test_option", test_opt);

    auto wall = make_ref<osg::ShapeDrawable>();
    wall->setShape( new osg::Box(osg::Vec3(-99.5f, 4.0f, 0.0f),
            1.0f, 8.0f, 200.0f) );
    wall->setColor( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );
    auto wall_geode = make_ref<osg::Geode>();
    wall_geode->addDrawable(wall);

    auto floor = make_ref<osg::ShapeDrawable>();
    floor->setShape( new osg::Box( osg::Vec3(0.0f, 0.0f, 0.0f), 200.0f, 0.5f, 200.0f) );
    auto floor_geode = make_ref<osg::Geode>();
    floor_geode->addDrawable(floor);

    auto main_obj = make_ref<osg::Group>();
    main_obj->addChild( floor_geode.get() );

    for(auto i = 0; i < 21; ++i)
    {
        auto transform = make_ref<osg::MatrixTransform>();
        transform->setMatrix( osg::Matrix::translate(i * 10, 0, 0));
        transform->addChild( wall_geode.get() );

        main_obj->addChild( transform.get() );
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
    init_rotate->addChild(spin);
    spin->addChild(pres_rotate);

    // Root group
    auto root = make_ref<osg::Group>();
    root->addChild(init_rotate);


    // Spin animation path
    auto spin_cp1 = osg::AnimationPath::ControlPoint();
    auto spin_cp2 = osg::AnimationPath::ControlPoint();
    auto spin_cp3 = osg::AnimationPath::ControlPoint();
    auto rotate1 = osg::Quat(0, osg::Y_AXIS);
    auto rotate2 = osg::Quat(osg::PI, osg::Y_AXIS);
    auto rotate3 = osg::Quat(osg::PI * 2, osg::Y_AXIS);
    spin_cp1.setRotation(rotate1);
    spin_cp2.setRotation(rotate2);
    spin_cp3.setRotation(rotate3);

    auto spin_path = make_ref<osg::AnimationPath>();
    spin_path->insert(0, spin_cp1);
    spin_path->insert(10, spin_cp2);
    spin_path->insert(20, spin_cp3);
    spin_path->setLoopMode(osg::AnimationPath::LOOP);

    // Set animation callback
    auto spin_callback = make_ref<osg::AnimationPathCallback>();
    spin_callback->setAnimationPath(spin_path.get());

    spin->addUpdateCallback(spin_callback.get());


    // Print node graph
    InfoVisitor info;
    root->accept(info);

    // Start viewer
    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
    return viewer.run();
}