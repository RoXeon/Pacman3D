#include "GhostFactory.h"
#include "make_ref.h"
#include "NPC.h"

#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <osg/Geode>

#include <algorithm>
#include <random>

osg::ref_ptr<osg::Node> GhostFactory::drawGhost(Board& board)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    double radius = std::min(board.getFieldSizeX(), board.getFieldSizeY()) / 1.5 / 2;
    auto rand = [](int i){ return std::rand()%i; };

    auto emptyFields = board.getEmptyFields();
    std::shuffle(emptyFields.begin(), emptyFields.end(), gen);

    uint32_t fx, fy;
    std::tie(fx, fy) = emptyFields[0];

    auto position = osg::Vec3d(board.getFieldCenterX(fx), board.getFieldCenterY(fy), radius * 1.5);

    auto ghostMain = make_ref<osg::Group>();
    auto ghostRootPos = make_ref<osg::MatrixTransform>(osg::Matrix::translate(position));
    auto ghostPath = make_ref<osg::MatrixTransform>();
    auto ghostRotation = make_ref<osg::MatrixTransform>();
    auto ghost = make_ref<osg::ShapeDrawable>();
    auto ghostGeode = make_ref<osg::Geode>();

    auto robot = osgDB::readNodeFile("/Users/RoXeon/Projects/Pacman3D/cow.osg");

    ghost->setShape(new osg::Sphere(osg::Vec3d{0, 0, 0}, radius));
    ghost->setColor(osg::Vec4(rand(2), rand(2), rand(2), rand(2)));

    ghostGeode->addDrawable(ghost);

    auto npc = make_ref<NPC>(&board, fx, fy);
    npc->addChild(robot);

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
