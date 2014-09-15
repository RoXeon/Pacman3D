#ifndef NPCMOVEMENTCALLBACK_H
#define NPCMOVEMENTCALLBACK_H

#include <osg/NodeCallback>

class NPCMovementCallback : public osg::NodeCallback
{
public:
    NPCMovementCallback() = default;
    NPCMovementCallback(const NPCMovementCallback& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
            : osg::NodeCallback(copy, copyop) {}
    META_Object(osg, NPCMovementCallback);

    virtual void operator() (osg::Node *node, osg::NodeVisitor *nv);
};

#endif // NPCMOVEMENTCALLBACK_H
