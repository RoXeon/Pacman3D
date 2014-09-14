#ifndef ghost_factory_h
#define ghost_factory_h

#include "Board.h"
#include "NPC.h"
#include "make_ref.h"

#include <osg/Geode>

class GhostFactory
{
public:
    osg::ref_ptr<osg::Node> drawGhost(Board& board);

private:
    class MovementVisitor : public osg::NodeVisitor
    {
    public:
        MovementVisitor()
        {
            setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
        };

        virtual void apply( osg::Geode& node )
        {
            if(std::string(node.className()) == "NPC")
            {
                auto& npc = dynamic_cast<NPC&>(node);
                auto animationPath = npc.getPathCallback()->getAnimationPath();

                auto last = animationPath ? animationPath->getLastTime() : 0;

                //std::cout << last << " " << npc.getPathCallback()->getAnimationTime() << std::endl;

                if(!animationPath || npc.getPathCallback()->getAnimationTime() > last) {

                    NPC::Direction* direction = npc.getDirection();
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

                        auto cp1 = osg::AnimationPath::ControlPoint(osg::Vec3(lastX - rootX, lastY - rootY, 0.0));
                        auto cp2 = osg::AnimationPath::ControlPoint(osg::Vec3(targetX - rootX, targetY - rootY, 0.0));

                        auto path = make_ref<osg::AnimationPath>();
                        path->insert(last, cp1);
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