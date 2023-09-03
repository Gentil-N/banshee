#include "game.hpp"

namespace bsh
{
       bool GAME_SHOULD_BE_ENDED;
       float GAME_DELTA;
       float GAME_ADVANCE;
       int WIN_WIDTH = 1600;
       int WIN_HEIGHT = 900;
       float WIN_RATIO = (float)WIN_WIDTH / (float)WIN_HEIGHT;
       float WIN_SIZE = 20.0f;
       float  WIN_LEFT = -WIN_SIZE * WIN_RATIO, 
              WIN_RIGHT = WIN_SIZE * WIN_RATIO, 
              WIN_BOTTOM = -WIN_SIZE,
              WIN_TOP = WIN_SIZE;
       float MAX_PLANE_ROLL = glm::pi<float>() / 3.5f;
       dsk::Window WINDOW;
       rp3d::PhysicsCommon PHCMN;
}