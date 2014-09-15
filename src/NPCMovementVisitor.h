#ifndef NPCMOVEMENTVISITOR_H
#define NPCMOVEMENTVISITOR_H

#include "NPC.h"

#include <osg/NodeVisitor>


class NPCMovementVisitor : public osg::NodeVisitor
{
public:
    NPCMovementVisitor()
    {
        setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
    };

    virtual void apply(osg::Node& node);

private:
    struct LastDirection : osg::Referenced
    {
        LastDirection(NPC::Direction* direct) : direction(direct) {};
        NPC::Direction* direction;
    };
};

#endif // NPCMOVEMENTVISITOR_H
