#include "dependensis.h"

#include <vector>

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);

    InitWindow(1600, 900, "Hello world");

    SetTargetFPS(60);
    DisableCursor();

    RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    RenderTexture2D fxaaTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    Shader shaders[6];
    shaders[0] = LoadShader(0, "raylib/shaders/posterization.fs");
    shaders[1] = LoadShader(0, "raylib/shaders/grayscale.fs");
    shaders[2] = LoadShader(0, "raylib/shaders/bloom.fs");
    shaders[3] = LoadShader(0, "raylib/shaders/blur.fs");
    shaders[4] = LoadShader(0, "raylib/shaders/dream_vision.fs");
    shaders[5] = LoadShader(0, "raylib/shaders/sobel.fs");

    int currentShader = 0;
    int shaderCount = 6;

    Shader fxaaShader = LoadShader(0, "raylib/shaders/fxaa.fs");
    
    World world;
    Player player({ 3, 1, 0 });
    Cube cube({ 0.0f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f });

    std::vector <Vector3> walls =
    {
        { 0.0f, 1, world.wallDist },
        { 0.0f, 1, -world.wallDist },
        {world.wallDist,1,0},
        {-world.wallDist,1,0}
    };

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_RIGHT)) currentShader = (currentShader + 1) % shaderCount;
        if (IsKeyPressed(KEY_LEFT)) currentShader = (currentShader - 1 + shaderCount) % shaderCount;

        float deltaTime = GetFrameTime();

        cube.update(player, world, deltaTime);
        player.update(world, deltaTime);


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
        SetShaderValue(fxaaShader, GetShaderLocation(fxaaShader, "resolution"), resolution, SHADER_UNIFORM_VEC2);

        DrawTextureRec(fxaaTexture.texture, { 0, 0, (float)fxaaTexture.texture.width, (float)-fxaaTexture.texture.height }, { 0, 0 }, WHITE);

        EndShaderMode();

        DrawFPS(10, 10);
        DrawText(".", GetScreenWidth() / 2, GetScreenHeight() / 2 - 11, 16, WHITE);

        EndDrawing();
    }

    UnloadRenderTexture(target);

    for (int i = 0; i < shaderCount; i++) UnloadShader(shaders[i]);

    CloseWindow();
    return 0;
}
