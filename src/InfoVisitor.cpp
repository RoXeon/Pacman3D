#include "InfoVisitor.h"

#include <iostream>
#include <osg/Geode>

InfoVisitor::InfoVisitor()
    : m_level(0)
{
    setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
}


std::string InfoVisitor::spaces()
{
    return std::string(m_level * 2, ' ');
}


void InfoVisitor::apply( osg::Node& node )
{
    std::cout << spaces() << node.libraryName() << "::" << node.className() << std::endl;

    m_level++;
    traverse( node );
    m_level--;
}


void InfoVisitor::apply( osg::Geode& geode )
{
    std::cout << spaces() << geode.libraryName() << "::"
            << geode.className() << std::endl;
    m_level++;
    for(auto drawable : geode.getDrawableList())
    {
        std::cout << spaces() << drawable->libraryName() << "::"
                << drawable->className() << std::endl;
    }

    traverse( geode );
    m_level--;
}