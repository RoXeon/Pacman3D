#include "GhostFactory.h"
#include "make_ref.h"
#include "NPC.h"

#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>

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

    auto ghostGroup = make_ref<osg::MatrixTransform>();
    auto ghost = make_ref<osg::ShapeDrawable>();
    ghost->setShape(new osg::Sphere(osg::Vec3d(board.getFieldCenterX(fx), board.getFieldCenterY(fy), radius * 1.5), radius));
    ghost->setColor(osg::Vec4(rand(2), rand(2), rand(2), rand(2)));
    auto npc = make_ref<NPC>(&board, fx, fy);
    npc->addDrawable(ghost);

    ghostGroup->addChild(npc);
    auto movement = make_ref<NPCMovementCallback>();
    ghostGroup->addUpdateCallback(movement.get());

    // Set animation callback
    auto pathCallback = make_ref<osg::AnimationPathCallback>();
    ghostGroup->addUpdateCallback(pathCallback);
    npc->setPathCallback(pathCallback);

    return ghostGroup;
}
