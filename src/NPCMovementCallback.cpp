#include "NPCMovementCallback.h"
#include "NPCMovementVisitor.h"

void NPCMovementCallback::operator() (osg::Node *node, osg::NodeVisitor *nv)
{
    auto visitor = NPCMovementVisitor();
    node->accept(visitor);

    traverse(node, nv);
}
