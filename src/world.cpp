#include "world.h"

void World::drawWorld(std::vector <Vector3> walls)
{
    DrawPlane({ 0, -1, 0 }, { 20, 20 }, DARKGRAY);

    if (walls.size() >= 4) 
    {
        DrawCube(walls[0], 10, 4, 0.1, DARKGREEN);
        DrawCube(walls[1], 10, 4, 0.1, DARKGREEN);
        DrawCube(walls[2], 0.1, 4, 10, DARKGREEN);
        DrawCube(walls[3], 0.1, 4, 10, DARKGREEN);
    }
}
