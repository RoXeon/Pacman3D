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
};

#endif // ghost_factory_h
