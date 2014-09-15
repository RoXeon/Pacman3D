#include "GhostFactory.h"
#include "make_ref.h"
#include "NPCMovementCallback.h"

#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>

#include <algorithm>
#include <random>

osg::ref_ptr<osg::Node> GhostFactory::drawGhost(Board& board, osg::Node* ghostModel)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Prepare model
    osg::Group* ModelGroup = ghostModel->asGroup();
    osg::Geode* ModelGeode = ModelGroup->getChild(0)->asGeode();
    osg::Geometry* ModelGeometry = ModelGeode->getDrawable(0)->asGeometry();

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4(139.0/255, 69.0/255, 19.0/255, 1.0) );

    ModelGeometry->setColorArray( colors );
    ModelGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    ghostModel->setCullingActive(true);

    double radius = std::min(board.getFieldSizeX(), board.getFieldSizeY()) / 1.5 / 2;
    auto rand = [](int i){ return std::rand()%i; };

    auto emptyFields = board.getEmptyFields();
    std::shuffle(emptyFields.begin(), emptyFields.end(), gen);

    uint32_t fx, fy;
    std::tie(fx, fy) = emptyFields[0];

    auto position = osg::Vec3d(board.getFieldCenterX(fx), board.getFieldCenterY(fy), radius);

    auto ghostMain = make_ref<osg::Group>();
    auto ghostRootPos = make_ref<osg::MatrixTransform>(osg::Matrix::translate(position));
    auto ghostPath = make_ref<osg::MatrixTransform>();
    auto ghostRotation = make_ref<osg::MatrixTransform>();
    auto ghost = make_ref<osg::ShapeDrawable>();
    auto ghostGeode = make_ref<osg::Geode>();

//    ghost->setShape(new osg::Sphere(osg::Vec3d{0, 0, 0}, radius));
//    ghost->setColor(osg::Vec4(rand(2), rand(2), rand(2), rand(2)));
//    ghostGeode->addDrawable(ghost);

    auto npc = make_ref<NPC>(&board, fx, fy);
    npc->addChild(ghostModel);

    ghostRotation->addChild(npc);
    ghostRootPos->addChild(ghostRotation);
    ghostPath->addChild(ghostRootPos);
    ghostMain->addChild(ghostPath);

    auto movement = make_ref<NPCMovementCallback>();
    ghostPath->addUpdateCallback(movement.get());

    // Set animation callback
    auto pathCallback = make_ref<osg::AnimationPathCallback>();
    ghostPath->addUpdateCallback(pathCallback);
    npc->setPathCallback(pathCallback);

    auto rotationCallback = make_ref<osg::AnimationPathCallback>();
    ghostRotation->addUpdateCallback(rotationCallback);
    npc->setRotationCallback(rotationCallback);

    return ghostMain;
}
