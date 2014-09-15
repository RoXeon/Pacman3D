#include "NPCMovementVisitor.h"
#include "make_ref.h"

#include <osgAnimation/Animation>


void NPCMovementVisitor::apply(osg::Node& node)
{
    if(std::string(node.className()) == "NPC")
    {
        auto& npc = dynamic_cast<NPC&>(node);
        auto animationPath = npc.getPathCallback()->getAnimationPath();
        auto animationRotation = npc.getRotationCallback()->getAnimationPath();

        auto last = animationPath ? animationPath->getLastTime() : 0;
        auto lastRotation = animationRotation ? animationRotation->getLastTime() : 0;

        NPC::Direction* direction = npc.getDirection();

        LastDirection* lastDirection = dynamic_cast<LastDirection*>(npc.getRotationCallback()->getUserData());

        if(!lastDirection || lastDirection->direction != direction) {
            if(!npc.getRotationCallback()->getUserData()) {
                npc.getRotationCallback()->setUserData(new LastDirection(direction));
                lastDirection = dynamic_cast<LastDirection*>(npc.getRotationCallback()->getUserData());
            }

            auto rotation = osg::Quat(lastDirection->direction->orientation() + osg::PI_2, osg::Vec3(0.0, 0.0, 1.0));
            auto targetRotation = osg::Quat(direction->orientation() + osg::PI_2, osg::Vec3(0.0, 0.0, 1.0));

            npc.getRotationCallback()->setUserData(new LastDirection(direction));

            auto cp0 = osg::AnimationPath::ControlPoint();
            cp0.setRotation(rotation);

            auto cp1 = osg::AnimationPath::ControlPoint();
            cp1.setRotation(targetRotation);

            auto path = make_ref<osg::AnimationPath>();
            path->insert(last, cp0);
            path->insert(last + 0.5, cp1);
            path->setLoopMode(osg::AnimationPath::NO_LOOPING);

            npc.getRotationCallback()->setAnimationPath(path);
        }

        if(!animationPath || npc.getPathCallback()->getAnimationTime() > last) {

            uint32_t cx, cy, rx, ry, tx, ty;
            std::tie(cx, cy) = npc.getCurrentPosition();
            std::tie(rx, ry) = npc.getRootPosition();

            tx = cx + direction->modX();
            ty = cy + direction->modY();

            if(npc.getBoard()->getField(tx, ty) == Board::FIELD_EMPTY) {
                npc.moveTo(tx, ty);

                double rootX = npc.getBoard()->getFieldCenterX(rx);
                double rootY = npc.getBoard()->getFieldCenterY(ry);

                double lastX = npc.getBoard()->getFieldCenterX(cx);
                double lastY = npc.getBoard()->getFieldCenterY(cy);

                double targetX = npc.getBoard()->getFieldCenterX(tx);
                double targetY = npc.getBoard()->getFieldCenterY(ty);

                auto rotation = osg::Quat(direction->orientation(), osg::Vec3(0.0, 0.0, 1.0));
                auto cp0 = osg::AnimationPath::ControlPoint();
                cp0.setRotation(rotation);
                auto cp1 = osg::AnimationPath::ControlPoint(osg::Vec3(lastX - rootX, lastY - rootY, 0.0));
                auto cp2 = osg::AnimationPath::ControlPoint(osg::Vec3(targetX - rootX, targetY - rootY, 0.0));

                auto path = make_ref<osg::AnimationPath>();
                path->insert(last + 0.005, cp1);
                path->insert(last + 0.5, cp2);
                path->setLoopMode(osg::AnimationPath::NO_LOOPING);

                npc.getPathCallback()->setAnimationPath(path);
            } else {
                int16_t attempts = 6;
                uint32_t cx, cy;
                bool isCorrectDirection = false;

                do {
                    std::tie(cx, cy) = npc.getCurrentPosition();
                    auto NewDirection = npc.changeDirection();

                    auto nx = cx + NewDirection->modX();
                    auto ny = cx + NewDirection->modY();
                    isCorrectDirection = npc.getBoard()->getField(nx, ny) == Board::FIELD_EMPTY;
                } while(attempts-- && !isCorrectDirection);
            }

        }
    }

    traverse(node);
}
