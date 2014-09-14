#ifndef ghost_factory_h
#define ghost_factory_h

#include "Board.h"
#include "NPC.h"
#include "make_ref.h"

#include <osg/Geode>

class GhostFactory
{
public:
    osg::ref_ptr<osg::Node> drawGhost(Board& board, osg::Node* model);

private:

    struct LastDirection : osg::Referenced
    {
        LastDirection(NPC::Direction* direct) : direction(direct) {};
        NPC::Direction* direction;
    };

    class MovementVisitor : public osg::NodeVisitor
    {
    public:
        MovementVisitor()
        {
            setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
        };

        virtual void apply( osg::Node& node )
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
                        npc.changeDirection();
                    }

                }
            }

            traverse(node);
        }
    };


    class NPCMovementCallback : public osg::NodeCallback
    {
    public:
        NPCMovementCallback() = default;
        NPCMovementCallback(const NPCMovementCallback& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
                : osg::NodeCallback(copy, copyop) {}
        META_Object(osg, NPCMovementCallback);

        virtual void operator() (osg::Node *node, osg::NodeVisitor *nv)
        {
            auto npc = dynamic_cast<NPC*>(node);

            auto visitor = MovementVisitor();
            node->accept(visitor);

            traverse(node, nv);
        }
    };
};

#endif // ghost_factory_h