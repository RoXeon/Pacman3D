#ifndef board_def_h
#define board_def_h

#include <osg/ref_ptr>

namespace {
    typedef std::map<std::string, std::string> BoardTheme;
    constexpr const char* FLOOR_REPEAT = "floorRepeat";
    constexpr const char* FLOOR = "floor";
    constexpr const char* CEIL  = "ceil";
    constexpr const char* WALL  = "wall";

    const BoardTheme GrassTheme{{CEIL, "ceil.bmp"}, {FLOOR, "grass.jpg"}, {WALL, "green_brick_wall.jpg"}, {FLOOR_REPEAT, "128"}};
    const BoardTheme StoneTheme{{CEIL, "ceil.bmp"}, {FLOOR, "floor4_small.jpg"}, {WALL, "brick_wall.jpg"}, {FLOOR_REPEAT, "64"}};
    const BoardTheme BloodTheme{{CEIL, "ceil.bmp"}, {FLOOR, "floor6.jpg"}, {WALL, "wall6.jpg"}, {FLOOR_REPEAT, "32"}};

    double boardSizeX = 500.0;
    double boardSizeY = 500.0;

    constexpr bool FogEnabled               = true;
    constexpr float BoardObjectsShininess   = 0.4;
    constexpr uint32_t BoardWallLOD         = 1;
    constexpr uint32_t BoardFloorLOD        = 1;

    BoardTheme         ActiveTheme          = GrassTheme;

    // y (y field: 0, position: boardSizeY)
    // |
    // |
    // |
    // |
    // |
    // |
    // |
    // |
    // |
    // |
    // |
    // |
    // |
    // |
    // |
    // |
    // |-----------------------------------------------------> x
    // (x field: 0, position: 0)

    std::vector<std::string> boardDefinition = {

                    "**********************************",
                    "*               **               *",
                    "*               **               *",
                    "*  ****  *****  **  *****  ****  *",
                    "*  ****  *****  **  *****  ****  *",
                    "*  ****  *****  **  *****  ****  *",
                    "*                                *",
                    "*                                *",
                    "*  ****  **  ********  **  ****  *",
                    "*  ****  **  ********  **  ****  *",
                    "*        **     **     **        *",
                    "*        **     **     **        *",
                    "*******  *****  **  *****  *******",
                    "*******  *****  **  *****  *******",
                    "*******  **            **  *******",
                    "*******  **            **  *******",
                    "*******  **  ***  ***  **  *******",
                    "*******  **  *      *  **  *******",
                    "*            *      *            *",
                    "*            *      *            *",
                    "*******  **  *      *  **  *******",
                    "*******  **  ********  **  *******",
                    "*******  **            **  *******",
                    "*******  **            **  *******",
                    "*******  **  ********  **  *******",
                    "*******  **  ********  **  *******",
                    "*               **               *",
                    "*               **               *",
                    "*  ****  *****  **  *****  ****  *",
                    "*  ****  *****  **  *****  ****  *",
                    "*    **                    **    *",
                    "*    **                    **    *",
                    "***  **  **  ********  **  **  ***",
                    "***  **  **  ********  **  **  ***",
                    "*        **     **     **        *",
                    "*        **     **     **        *",
                    "*  ***********  **  ***********  *",
                    "*  ***********  **  ***********  *",
                    "*                                *",
                    "*                                *",
                    "**********************************"
    };

};

#endif // board_def_h
