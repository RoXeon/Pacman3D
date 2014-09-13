#ifndef InfoVisitor_h
#define InfoVisitor_h

#include <osg/NodeVisitor>

class InfoVisitor : public osg::NodeVisitor
{
public:
    InfoVisitor();

    virtual void apply( osg::Node& node );
    virtual void apply( osg::Geode& geode );
protected:
    unsigned int m_level;

    std::string spaces();
};

#endif // InfoVisitor_h