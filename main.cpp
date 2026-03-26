#include "raylib/include/raylib.h"
#include "src/cube.h"
#include "src/player.h"
#include "src/world.h"

#include <vector>

int main()
{
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	SetConfigFlags(FLAG_VSYNC_HINT);

	InitWindow(1280, 800, "Hello world");

	SetTargetFPS(60);
	DisableCursor();

	World world;

	Player player({ 3, 1, 0 });

	Cube cube({ 0.0f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f });

	std::vector <Vector3> walls = { { 0.0f, 1, cube.wallDist }, { 0.0f, 1, -cube.wallDist }, {cube.wallDist,1,0}, {-cube.wallDist,1,0} };

	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();

		cube.Physics(world, deltaTime);
		cube.Throw(player, world, deltaTime);
		cube.Pick(player, world, deltaTime);

		player.movemant(world, deltaTime);
		player.jump(world, deltaTime);


		BeginDrawing();
		ClearBackground(SKYBLUE);
		BeginMode3D(player.cam);

		world.drawWorld(walls);

		DrawCube(cube.position, cube.size.x, cube.size.y, cube.size.z, cube.color);
		
		EndMode3D();

		DrawFPS(10, 10);
		DrawText(".", GetScreenWidth() / 2, GetScreenHeight() / 2 - 11, 16, WHITE);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
