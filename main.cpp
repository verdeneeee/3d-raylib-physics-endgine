#include <iostream>

#include "raylib/include/raylib.h"

#include "raylib/include/raymath.h"

#define RLIGHTS_IMPLEMENTATION
#include "raylib/include/rlights.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

void throwCube(bool& isPicked, Vector3& cubeVelosity, Ray& ray, float throwForce, RayCollision& collision, float& deltaTime);
void pickCube(Camera3D& cam, bool& isPicked, bool& isCubeGrounded, Vector3& cubePos, Vector3& cubeSize, float& pickDist, Ray& ray, RayCollision& collision, Color& cubeCol, Vector3& cubeVelosity, float& gravity, int& cubeMass, float& deltaTime, Vector2& windowSize);
bool checkCollisionPlayerCube(Camera& cam, Vector3& cubePos, Vector3& cubeSize);

bool checkCollisionPlayerRightWall(Camera& cam);
bool checkCollisionPlayerLeftWall(Camera& cam);
bool checkCollisionPlayerFrontWall(Camera& cam);
bool checkCollisionPlayerBackWall(Camera& cam);

int main()
{
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	//SetConfigFlags(FLAG_VSYNC_HINT);
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);
	SetConfigFlags(FLAG_WINDOW_HIGHDPI); 

	Vector2 winSize{ 1280, 800 };

	InitWindow(winSize.x, winSize.y, "Hello world");
	SetTargetFPS(60);

	Camera3D cam = { 0 };
	cam.position = { 3,1,0 };
	cam.target = { 0,0,0 };
	cam.projection = CAMERA_PERSPECTIVE;
	cam.up = { 0,1,0 };
	cam.fovy = 90;

	Vector3 playerVelosity = { 0.0f,0.0f,0.0f };
	Vector3 cubePos = { 0,0.5,0 };
	Vector3 cubeSize = { 1, 1, 1 };
	Vector3 cubeVelosity = { 0.0f,0.0f,0.0f };

	float friction = 0.86f;

	float gravity = 32.0f;
	float jumpForce = 12.0f;
	bool isGrounded = 1;

	const int throwForce = 13;
	int cubeMass = 5;
	float pickDist = 0.0f;
	bool isPicked = 0;
	bool isCubeGrounded = 1;
	Color cubeCol = WHITE;

	Ray ray = { 0 };
	RayCollision collision = { 0 };


	Shader shader = LoadShader(TextFormat("raylib/shaders/lighting.vs", GLSL_VERSION),
					TextFormat("raylib/shaders/lighting.fs", GLSL_VERSION));

	shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
	int ambientLoc = GetShaderLocation(shader, "ambient");
		
	Vector4 ambientColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	SetShaderValue(shader, ambientLoc, &ambientColor, SHADER_UNIFORM_VEC4);

	Light light = { CreateLight(LIGHT_POINT, { 0, 7, 0 }, {0, 0, 0}, RAYWHITE, shader) };

	DisableCursor();

	while (!WindowShouldClose())
	{
		cam.up = { 0,1,0 };
		float deltaTime = GetFrameTime();

		cam.fovy = 90;

		Vector3 oldCamPos = cam.position;

		SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &oldCamPos, SHADER_UNIFORM_VEC3);

		Vector2 mouseDelta = GetMouseDelta();
		Vector3 moveDir = { 0 };

		float accelerate = 1.5f;

		if (IsKeyDown(KEY_W))
		{
			if (IsKeyDown(KEY_LEFT_CONTROL)) 
			{ 
				cam.fovy = 100;
				accelerate += 2.0f;
			}
			playerVelosity.x += accelerate * deltaTime;
		}

		if (IsKeyDown(KEY_S))
		{
			if (IsKeyDown(KEY_LEFT_CONTROL)) 
			{
				cam.fovy = 100;
				accelerate += 2.0f;
			}
			playerVelosity.x -= accelerate * deltaTime;
		}

		if (IsKeyDown(KEY_D))
		{
			if (IsKeyDown(KEY_LEFT_CONTROL)) 
			{
				cam.fovy = 100;
				accelerate += 2.0f;
			}
			playerVelosity.z += accelerate * deltaTime;
		}

		if (IsKeyDown(KEY_A))
		{
			if (IsKeyDown(KEY_LEFT_CONTROL)) 
			{
				cam.fovy = 100;
				accelerate += 2.0f;
			}
			playerVelosity.z -= accelerate * deltaTime;
		}
		playerVelosity.x *= friction;
		playerVelosity.z *= friction;

		float maxSpeed = 0.5f;

		if (playerVelosity.x > maxSpeed) playerVelosity.x = maxSpeed;
		if (playerVelosity.x < -maxSpeed) playerVelosity.x = -maxSpeed;
		if (playerVelosity.z > maxSpeed) playerVelosity.z = maxSpeed;
		if (playerVelosity.z < -maxSpeed) playerVelosity.z = -maxSpeed;

		if (IsKeyDown(KEY_SPACE) && isGrounded)
		{
			isGrounded = 0;
			playerVelosity.y = jumpForce;
		}

		playerVelosity.y -= gravity * deltaTime;
		cam.position.y += playerVelosity.y * deltaTime;

		if (cam.position.y <= 0)
		{
			cam.position.y = 0;
			playerVelosity.y = 0.0f;
			isGrounded = true;
		}

		
		throwCube(isPicked, cubeVelosity, ray, throwForce, collision, deltaTime);
		pickCube(cam, isPicked, isCubeGrounded, cubePos, cubeSize, pickDist, ray, collision, cubeCol, cubeVelosity, gravity, cubeMass, deltaTime, winSize);

		Vector3 nextPos = Vector3Add(cam.position, playerVelosity);

		UpdateCameraPro(&cam,
			{ playerVelosity.x, playerVelosity.z, 0.0f },
			{ GetMouseDelta().x * 0.13f, GetMouseDelta().y * 0.13f }, 0);


		cam.target.y += playerVelosity.y * deltaTime;

		if (checkCollisionPlayerCube(cam, cubePos, cubeSize))
		{
			Vector3 surfPos = cam.position;
			cam.position = oldCamPos;

			cam.position.x = surfPos.x;
			if (checkCollisionPlayerCube(cam, cubePos, cubeSize))
			{
				playerVelosity.x = 0;
				cam.position.x = oldCamPos.x;
			}

			cam.position.y = surfPos.y;
			if (checkCollisionPlayerCube(cam, cubePos, cubeSize))
			{
				isGrounded = 1;
				playerVelosity.y = 0;
				cam.position.y = oldCamPos.y;
			}

			cam.position.z = surfPos.z;
			if (checkCollisionPlayerCube(cam, cubePos, cubeSize))
			{
				playerVelosity.z = 0;
				cam.position.z = oldCamPos.z;
			}
		}

		if (checkCollisionPlayerRightWall(cam))
		{
			Vector3 surfPos = cam.position;
			cam.position = oldCamPos;

			cam.position.x = surfPos.x;
			if (checkCollisionPlayerRightWall(cam))
			{
				playerVelosity.x = 0;
				cam.position.x = oldCamPos.x;
			}

			cam.position.y = surfPos.y;
			if (checkCollisionPlayerRightWall(cam))
			{
				isGrounded = 1;
				playerVelosity.y = 0;
				cam.position.y = oldCamPos.y; 
			}

			cam.position.z = surfPos.z;
			if (checkCollisionPlayerRightWall(cam))
			{ 
				playerVelosity.z = 0;
				cam.position.z = oldCamPos.z;
			}
		}

		if (checkCollisionPlayerLeftWall(cam))
		{
			Vector3 surfPos = cam.position; 
			cam.position = oldCamPos;

			cam.position.x = surfPos.x;
			if (checkCollisionPlayerLeftWall(cam)) 
			{
				playerVelosity.x = 0;
				cam.position.x = oldCamPos.x;
			}

			cam.position.y = surfPos.y;
			if (checkCollisionPlayerLeftWall(cam)) 
			{
				isGrounded = 1;
				playerVelosity.y = 0; 
				cam.position.y = oldCamPos.y; 
			}

			cam.position.z = surfPos.z;
			if (checkCollisionPlayerLeftWall(cam)) 
			{ 
				playerVelosity.z = 0;
				cam.position.z = oldCamPos.z;
			}
		}

		if (checkCollisionPlayerFrontWall(cam))
		{
			Vector3 surfPos = cam.position;
			cam.position = oldCamPos;

			cam.position.x = surfPos.x;
			if (checkCollisionPlayerFrontWall(cam)) 
			{ 
				playerVelosity.x = 0;
				cam.position.x = oldCamPos.x;
			}

			cam.position.y = surfPos.y;
			if (checkCollisionPlayerFrontWall(cam)) 
			{
				isGrounded = 1;
				playerVelosity.y = 0;
				cam.position.y = oldCamPos.y;
			}

			cam.position.z = surfPos.z;
			if (checkCollisionPlayerFrontWall(cam)) 
			{ 
				playerVelosity.z = 0;
				cam.position.z = oldCamPos.z; 
			}
		}

		if (checkCollisionPlayerBackWall(cam))
		{
			Vector3 surfPos = cam.position; 
			cam.position = oldCamPos;

			cam.position.x = surfPos.x;
			if (checkCollisionPlayerBackWall(cam)) 
			{ 
				playerVelosity.x = 0; 
				cam.position.x = oldCamPos.x;
			}

			cam.position.y = surfPos.y;
			if (checkCollisionPlayerBackWall(cam)) 
			{
				isGrounded = 1;
				playerVelosity.y = 0; 
				cam.position.y = oldCamPos.y; 
			}

			cam.position.z = surfPos.z;
			if (checkCollisionPlayerBackWall(cam)) 
			{ 
				playerVelosity.z = 0;
				cam.position.z = oldCamPos.z;
			}
		}

		BeginDrawing();	
		BeginMode3D(cam);
		BeginShaderMode(shader);
		ClearBackground(SKYBLUE);

		
		DrawCube(cubePos, cubeSize.x, cubeSize.y, cubeSize.z, cubeCol);


		DrawPlane({ 0,-1,0 }, { 10,10 }, DARKGRAY);

		DrawCube({ 0.0f, 1, -5.0f }, 10.0f, 4.0f, 0.1f, DARKGREEN);
		DrawCube({ 0.0f, 1,  5.0f }, 10.0f, 4.0f, 0.1f, DARKGREEN);

		DrawCube({ -5.0f, 1, 0.0f }, 0.1f, 4.0f, 10.0f, DARKGREEN);
		DrawCube({ 5.0f, 1, 0.0f }, 0.1f, 4.0f, 10.0f, DARKGREEN);

		EndShaderMode();

		EndMode3D();

		DrawFPS(10, 10);

		DrawText(".", winSize.x / 2, winSize.y / 2 - 11 , 16, WHITE);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}

void throwCube(bool& isPicked, Vector3& cubeVelosity, Ray& ray, float throwForce, RayCollision &collision, float &deltaTime)
{
	if (IsKeyPressed(KEY_E) && isPicked)
	{
		isPicked = 0;
		collision.hit = 0;

		cubeVelosity.x = ray.direction.x * throwForce;
		cubeVelosity.y = ray.direction.y * throwForce;
		cubeVelosity.z = ray.direction.z * throwForce;
	}
}

void pickCube(Camera3D& cam, bool& isPicked, bool& isCubeGrounded, Vector3& cubePos, Vector3& cubeSize, float& pickDist, Ray& ray, RayCollision& collision, Color &cubeCol, Vector3 &cubeVelosity, float &gravity, int &cubeMass, float &deltaTime, Vector2 &windowSize)
{
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		if (!collision.hit)
		{
			ray = GetScreenToWorldRay({ windowSize.x / 2, windowSize.y / 2 }, cam);

			collision = GetRayCollisionBox(ray,
				{ { cubePos.x - cubeSize.x / 2, cubePos.y - cubeSize.y / 2, cubePos.z - cubeSize.z / 2},
				{cubePos.x + cubeSize.x / 2, cubePos.y + cubeSize.y / 2, cubePos.z + cubeSize.z / 2} });
		}
		else collision.hit = false;
	}

	if (isPicked)
	{
		ray = GetScreenToWorldRay({ windowSize.x / 2, windowSize.y / 2 }, cam);
		//cubeCol = LIME;

		cubeVelosity = { 0,0,0 };

		collision.distance += GetMouseWheelMove() * 0.5f;
		if (collision.distance < 2.0f) collision.distance = 2.0f;

		cubePos.x = ray.position.x + ray.direction.x * pickDist;
		cubePos.y = ray.position.y + ray.direction.y * pickDist;
		cubePos.z = ray.position.z + ray.direction.z * pickDist;

		isCubeGrounded = 0;
	}

	else
	{
		cubeCol = WHITE;

		if (!isCubeGrounded)
		{
			cubeVelosity.y -= (gravity + cubeMass) * deltaTime;

			cubePos.x += cubeVelosity.x * deltaTime;
			cubePos.y += cubeVelosity.y * deltaTime;
			cubePos.z += cubeVelosity.z * deltaTime;

			float wall = 5.0f;

			if (cubePos.x + cubeSize.x / 2.0f > wall) { cubePos.x = wall - cubeSize.x / 2.0f; cubeVelosity.x *= -0.2f; }
			if (cubePos.x - cubeSize.x / 2.0f < -wall) { cubePos.x = -wall + cubeSize.x / 2.0f; cubeVelosity.x *= -0.2f; }
			if (cubePos.z + cubeSize.x / 2.0f > wall) {cubePos.z = wall - cubeSize.x / 2.0f; cubeVelosity.z *= -0.2f; }
			if (cubePos.z - cubeSize.x / 2.0f < -wall) { cubePos.z = -wall + cubeSize.x / 2.0f; cubeVelosity.z *= -0.2f;}
		}
		if (cubePos.y < -0.5)
		{
			cubePos.y = -0.5;
			cubeVelosity.y = 0;
			isCubeGrounded = 1;
		}
	}

	if (cubePos.y <= -1) isCubeGrounded = 1;

	if (collision.hit)
	{
		isPicked = 1;
		pickDist = collision.distance;
	}
	else isPicked = 0;
}

bool checkCollisionPlayerCube(Camera &cam, Vector3 &cubePos, Vector3 &cubeSize)
{
	if (CheckCollisionBoxes({ { cam.position.x - 0.1f / 2.0f, cam.position.y - 2.0f / 2.0f, cam.position.z - 0.1f / 2.0f}, {cam.position.x + 0.1f / 2.0f, cam.position.y + 2.0f / 2.0f, cam.position.z + 0.1f / 2.0f,} }, { { cubePos.x - cubeSize.x / 2, cubePos.y - cubeSize.y / 2, cubePos.z - cubeSize.z / 2}, {cubePos.x + cubeSize.x / 2, cubePos.y + cubeSize.y / 2, cubePos.z + cubeSize.z / 2} }))
	{
		return true;
	}
	return false;
}

bool checkCollisionPlayerRightWall(Camera& cam)
{
	if (CheckCollisionBoxes({ { cam.position.x - 0.05f, cam.position.y - 1.0f, cam.position.z - 0.05f}, {cam.position.x + 0.05f, cam.position.y + 1.0f, cam.position.z + 0.05f} }, { { -5.0f, -1.0f, -5.05f }, { 5.0f, 3.0f, -4.95f } }))
	{
		return true;
	}
	return false;
}

bool checkCollisionPlayerLeftWall(Camera& cam)
{
	if (CheckCollisionBoxes({ { cam.position.x - 0.05f, cam.position.y - 1.0f, cam.position.z - 0.05f}, {cam.position.x + 0.05f, cam.position.y + 1.0f, cam.position.z + 0.05f} }, { { -5.0f, -1.0f, 4.95f }, { 5.0f, 3.0f, 5.05f } }))
	{
		return true;
	}
	return false;
}

bool checkCollisionPlayerFrontWall(Camera& cam)
{
	if (CheckCollisionBoxes({ { cam.position.x - 0.05f, cam.position.y - 1.0f, cam.position.z - 0.05f}, {cam.position.x + 0.05f, cam.position.y + 1.0f, cam.position.z + 0.05f} }, { { -5.05f, -1.0f, -5.0f }, { -4.95f, 3.0f, 5.0f } }))
	{
		return true;
	}
	return false;
}

bool checkCollisionPlayerBackWall(Camera& cam)
{
	if (CheckCollisionBoxes({ { cam.position.x - 0.05f, cam.position.y - 1.0f, cam.position.z - 0.05f}, {cam.position.x + 0.05f, cam.position.y + 1.0f, cam.position.z + 0.05f} }, { { 4.95f, -1.0f, -5.0f }, { 5.05f, 3.0f, 5.0f } }))
	{
		return true;
	}
	return false;
}
