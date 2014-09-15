cmake_minimum_required(VERSION 3.0.0)
project(Pacman3D)

# enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenThreads)
find_package(osg)
find_package(osgDB)
find_package(osgGA)
find_package(osgUtil)
find_package(osgViewer)

macro(config_project PROJNAME LIBNAME)
    include_directories(${${LIBNAME}_INCLUDE_DIR})
    target_link_libraries(${PROJNAME} ${${LIBNAME}_LIBRARY})
endmacro()

set(SOURCE_FILES
    src/main.cpp
    src/InfoVisitor.cpp src/Board.cpp src/NPC.cpp src/GhostFactory.cpp src/board_def.h src/FPSManipulator.cpp
    src/NPCMovementCallback.cpp src/NPCMovementVisitor.cpp
)
add_executable(Pacman3D ${SOURCE_FILES})

config_project(Pacman3D OPENTHREADS)
config_project(Pacman3D OSG)
config_project(Pacman3D OSGDB)
config_project(Pacman3D OSGGA)
config_project(Pacman3D OSGUTIL)
config_project(Pacman3D OSGVIEWER)
config_project(Pacman3D OSGGA)
