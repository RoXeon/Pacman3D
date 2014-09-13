#ifndef board_utils_h
#define board_utils_h

#include <vector>
#include <string>

#include <osg/Group>
#include <osg/ref_ptr>

std::vector<std::vector<bool>> initBoard(std::vector<std::string>& def);
osg::ref_ptr<osg::Group> drawBoard(double sizeX, double sizeY, std::vector<std::vector<bool>>&& board);

#endif // board_utils_h