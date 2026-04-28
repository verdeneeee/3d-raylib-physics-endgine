#include "engine.h"

Game::Game() : player({ 3, 1, 0 }), cube({ 0.0f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f })
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    InitWindow(1600, 900, "Hello world");

    SetTargetFPS(60);
    DisableCursor();

    target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    fxaaTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    shaders[0] = LoadShader(0, "shaders/posterization.fs");
    shaders[1] = LoadShader(0, "shaders/grayscale.fs");
    shaders[2] = LoadShader(0, "shaders/bloom.fs");
    shaders[3] = LoadShader(0, "shaders/blur.fs");
    shaders[4] = LoadShader(0, "shaders/dream_vision.fs");
    shaders[5] = LoadShader(0, "shaders/sobel.fs");

    currentShader = 0;
    shaderCount = 6;

    fxaaShader = LoadShader(0, "shaders/fxaa.fs");
    resLoc = GetShaderLocation(fxaaShader, "resolution");

    walls =
    {
        { 0.0f, 1, world.wallDist },                 // x:0, y:1, z:5
        { 0.0f, 1, -world.wallDist },                // x:0, y:1, z:-5
        {world.wallDist,1,0.0f},                     // x:5, y:1, z:0
        {-world.wallDist,1,0.0f}                     // x:-5, y:1, z:0
    };
}

void Game::run()
{
    while (!WindowShouldClose())
    {
        Update();
        Draw();
    }
}

void Game::Update()
{
    float deltaTime = GetFrameTime();

    if (IsKeyPressed(KEY_RIGHT)) currentShader = (currentShader + 1) % shaderCount;
    if (IsKeyPressed(KEY_LEFT)) currentShader = (currentShader - 1 + shaderCount) % shaderCount;

    cube.update(player, world, deltaTime);
    player.update(world, deltaTime);
}

void Game::Draw()
{
    BeginTextureMode(target);
        ClearBackground(SKYBLUE);
        BeginMode3D(player.cam);
            world.drawWorld(walls);
            DrawCube(cube.position, cube.size.x, cube.size.y, cube.size.z, cube.color);
        EndMode3D();
    EndTextureMode();

    BeginTextureMode(fxaaTexture);
        BeginShaderMode(shaders[currentShader]);
            DrawTextureRec(target.texture, { 0, 0, (float)target.texture.width, (float)-target.texture.height }, { 0, 0 }, WHITE);
        EndShaderMode();
    EndTextureMode();

    BeginDrawing();
        BeginShaderMode(fxaaShader);
            float resolution[2] = { (float)GetScreenWidth(), (float)GetScreenHeight() };
            SetShaderValue(fxaaShader, resLoc, resolution, SHADER_UNIFORM_VEC2);
            DrawTextureRec(fxaaTexture.texture, { 0, 0, (float)fxaaTexture.texture.width, (float)-fxaaTexture.texture.height }, { 0, 0 }, WHITE);
        EndShaderMode();

        DrawFPS(10, 10);
        DrawText(".", GetScreenWidth() / 2, GetScreenHeight() / 2 - 11, 16, WHITE);
    EndDrawing();
}

Game::~Game()
{
    UnloadRenderTexture(target);
    UnloadRenderTexture(fxaaTexture);

    for (int i = 0; i < 6; i++) UnloadShader(shaders[i]);
    UnloadShader(fxaaShader);

    CloseWindow();
}
