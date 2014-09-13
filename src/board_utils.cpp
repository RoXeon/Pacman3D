#include "board_utils.h"
#include "make_ref.h"

#include <string>
#include <vector>
#include <cassert>

#include <osg/ShapeDrawable>
#include <osg/Geode>

std::vector<std::vector<bool>> initBoard(std::vector<std::string>& def)
{
    assert(def.size() > 0);
    std::vector<std::vector<bool>> board(def.size());

    auto height = def.size();
    auto width = def[0].size();

    for(auto elem : def) {
        assert(elem.size() >= width);
    }

    for(auto& elem : board) {
        elem.resize(width, false);
    }

    for(auto i = 0; i < height; ++i)
        for(auto j = 0; j < width; ++j)
            board[i][j] = def[i][j] == '*';

    return std::move(board);
}

osg::ref_ptr<osg::Group> drawBoard(double sizeX, double sizeY, std::vector<std::vector<bool>>&& board)
{


    auto normalize = osg::Vec3d(-sizeX / 2, -sizeY / 2, 0.0f);

    double blockSizeY = sizeY / board.size();
    double blockSizeX = sizeX / board[0].size();
    double blockSizeZ = 5 * std::max(blockSizeY, blockSizeX);

    auto overlaySize = std::min(blockSizeX, blockSizeY) / 100;

    auto boardObj = make_ref<osg::Group>();


    for(auto i = 0; i < board.size(); ++i) { // y
        for(auto j = 0; j < board[i].size(); ++j) { // x
            if(board[i][j]) {
                auto center = osg::Vec3d(blockSizeX * j + blockSizeX / 2, sizeY - blockSizeY * i - blockSizeY / 2, blockSizeZ / 2) - normalize;
                auto dr = make_ref<osg::ShapeDrawable>();
                dr->setShape(new osg::Box(center, blockSizeX + overlaySize, blockSizeY + overlaySize, blockSizeZ) );



                dr->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
                auto drgd = make_ref<osg::Geode>();
                drgd->addDrawable(dr);

                boardObj->addChild(drgd.get());
            }
        }
    }

    auto floor = make_ref<osg::ShapeDrawable>();
    floor->setShape( new osg::Box( osg::Vec3(sizeX / 2, sizeY / 2, 0.0f) - normalize, sizeX, sizeY, 0.5f) );
    auto floor_geode = make_ref<osg::Geode>();
    floor_geode->addDrawable(floor);

    boardObj->addChild(floor_geode);

    return boardObj;
}