#ifndef npc_h
#define npc_h

#include "Board.h"

#include <cstdlib>

#include <osg/Geode>
#include <osg/AnimationPath>

typedef uint64_t npc_id_type;

class NPC : public osg::Geode
{
public:
    struct Direction {
        virtual int16_t modX() = 0;
        virtual int16_t modY() = 0;
    };
    struct DirectionUpSt      : Direction { int16_t modX() { return  0; } int16_t modY() { return  1; } } DirectionUp;
    struct DirectionDownSt    : Direction { int16_t modX() { return  0; } int16_t modY() { return -1; } } DirectionDown;
    struct DirectionLeftSt    : Direction { int16_t modX() { return -1; } int16_t modY() { return  0; } } DirectionLeft;
    struct DirectionRightSt   : Direction { int16_t modX() { return  1; } int16_t modY() { return  0; } } DirectionRight;

    NPC() = default;
    NPC(Board* board, uint32_t rootX, uint32_t rootY);

    NPC(const NPC& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
            : osg::Geode(copy, copyop), m_id(copy.m_id) {}
    META_Object( osg, NPC );


    void setId(const npc_id_type id);

    Direction* changeDirection();
    Direction* getDirection();
    void moveTo(uint32_t x, uint32_t y);

    Board* getBoard() const;
    std::tuple<uint32_t, uint32_t> getCurrentPosition() const;
    std::tuple<uint32_t, uint32_t> getRootPosition() const;

    void setPathCallback(osg::ref_ptr<osg::AnimationPathCallback> pathCallback);
    osg::ref_ptr<osg::AnimationPathCallback> getPathCallback();

private:
    npc_id_type m_id;
    osg::ref_ptr<osg::AnimationPathCallback> m_pathCallback;
    Direction* m_direction;

    uint32_t m_rootX;
    uint32_t m_rootY;
    uint32_t m_cx;
    uint32_t m_cy;

    Board*   m_board;
};


#endif // npc_h