#include "NPC.h"

#include <algorithm>
#include <random>

NPC::NPC(Board* board, uint32_t rootX, uint32_t rootY)
    : m_rootX(rootX)
    , m_rootY(rootY)
    , m_cx(rootX)
    , m_cy(rootY)
    , m_board(board)
{
    m_board->setField(rootX, rootY, Board::FIELD_NPC);
    changeDirection();
}

Board* NPC::getBoard() const
{
    return m_board;
}

NPC::Direction* NPC::changeDirection()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::vector<NPC::Direction*> directions = { &DirectionUp, &DirectionDown, &DirectionLeft, &DirectionRight };
    std::shuffle(directions.begin(), directions.end(), gen);

    return (m_direction = directions[0]);
}

NPC::Direction* NPC::getDirection()
{
    return m_direction;
}

void NPC::setId(const npc_id_type id)
{
    m_id = id;
}

void NPC::moveTo(uint32_t x, uint32_t y)
{
    m_board->setField(m_cx, m_cy, Board::FIELD_EMPTY);
    m_board->setField(x, y, Board::FIELD_NPC);

    m_cx = x;
    m_cy = y;
}

std::tuple<uint32_t, uint32_t> NPC::getCurrentPosition() const
{
    return std::tuple<uint32_t, uint32_t>{m_cx, m_cy};
}

std::tuple<uint32_t, uint32_t> NPC::getRootPosition() const
{
    return std::tuple<uint32_t, uint32_t>{m_rootX, m_rootY};
}

void NPC::setPathCallback(osg::ref_ptr<osg::AnimationPathCallback> pathCallback)
{
    m_pathCallback = pathCallback;
}

osg::ref_ptr<osg::AnimationPathCallback> NPC::getPathCallback()
{
    return m_pathCallback;
}
