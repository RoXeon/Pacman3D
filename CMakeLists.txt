cmake_minimum_required(VERSION 3.0.0)
project(Pacman3D)

# enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenThreads)
find_package(osg)
find_package(osgDB)
find_package(osgUtil)
find_package(osgViewer)
find_package(osgGA)

macro(config_project PROJNAME LIBNAME)
    include_directories(${${LIBNAME}_INCLUDE_DIR})
    target_link_libraries(${PROJNAME} ${${LIBNAME}_LIBRARY})
endmacro()

set(SOURCE_FILES src/main.cpp src/InfoVisitor.cpp src/board_utils.cpp src/Board.cpp src/NPC.cpp src/GhostFactory.cpp)
add_executable(Pacman3D ${SOURCE_FILES})

config_project(Pacman3D OPENTHREADS)
config_project(Pacman3D OSG)
config_project(Pacman3D OSGDB)
config_project(Pacman3D OSGUTIL)
config_project(Pacman3D OSGVIEWER)
config_project(Pacman3D OSGGA)