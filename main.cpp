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

typedef struct {
	Vector3 position;
	Vector3 velocity;
	Vector3 dir;
	bool isGrounded;
} Body;

static Body player = { 0 };

void throwCube(bool& isPicked, Vector3& cubeVelosity, Ray& ray, float throwForce, RayCollision& collision);
void pickCube(Camera3D& cam, bool& isPicked, bool& isCubeGrounded, Vector3& cubePos, Vector3& cubeSize, float& pickDist, Ray& ray, RayCollision& collision, Color& cubeCol, Vector3& cubeVelosity, float& gravity, int& cubeMass, float& deltaTime, Vector2& windowSize);
static void UpdateCameraFPS(Camera* camera, Vector2 lookRotation, Vector2 lean, float headTimer, float walkLerp);
void UpdateBody(Body* body, float rot, char side, char forward, bool jumpPressed, bool crouchHold, float gravity, float jumpForce, float control, float airDrag, float maxSpeed, float maxAccel, float crouchSpeed, float friction);


int main()
{
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	SetConfigFlags(FLAG_VSYNC_HINT);
	//SetConfigFlags(FLAG_WINDOW_HIGHDPI);
	SetConfigFlags(FLAG_WINDOW_UNDECORATED);
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
	Vector3 currentSpeed = { 0,0,0 };

	Vector2 lookRotation = { 0 };
	float headTimer = 0.0f;
	float walkLerp = 0.0f;
	float headLerp = 1.0f;
	Vector2 lean = { 0 };
	Vector2 sensitivity = { 0.001f, 0.001f };

	float control = 15.0f;

	float friction = 0.86f;
	float airDrag = 0.98f;
	float maxAccel = 150.0f;
	float maxSpeed = 20.0f;
	float crouchSpeed = 5.0f;

	float gravity = 50.0f;
	float jumpForce = 15.0f;
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

		/*cam.fovy = 90;

		Vector3 oldCamPos = cam.position;

		SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &oldCamPos, SHADER_UNIFORM_VEC3);

		float deltaTime = GetFrameTime();

		Vector2 mouseDelta = GetMouseDelta();
		Vector3 moveDir = { 0 };

		accelerate = 1.5f;

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
		}*/

		Vector2 mouseDelta = GetMouseDelta();
		lookRotation.x -= mouseDelta.x * sensitivity.x;
		lookRotation.y += mouseDelta.y * sensitivity.y;

		char sideway = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
		char forward = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
		bool crouching = IsKeyDown(KEY_LEFT_CONTROL);
		UpdateBody(&player, lookRotation.x, sideway, forward, IsKeyPressed(KEY_SPACE), crouching, gravity, jumpForce, control, airDrag, maxSpeed, maxAccel, crouchSpeed, friction);

		float delta = GetFrameTime();
		headLerp = Lerp(headLerp, (crouching ? 0.0f : 1.0f), 20.0f * delta);
		cam.position = {
			player.position.x,
			player.position.y + (0.5f + headLerp),
			player.position.z,
		};

		if (player.isGrounded && ((forward != 0) || (sideway != 0)))
		{
			headTimer += delta * 3.0f;
			walkLerp = Lerp(walkLerp, 1.0f, 10.0f * delta);
			cam.fovy = Lerp(cam.fovy, 55.0f, 5.0f * delta);
		}
		else
		{
			walkLerp = Lerp(walkLerp, 0.0f, 10.0f * delta);
			cam.fovy = Lerp(cam.fovy, 60.0f, 5.0f * delta);
		}

		lean.x = Lerp(lean.x, sideway * 0.02f, 10.0f * delta);
		lean.y = Lerp(lean.y, forward * 0.015f, 10.0f * delta);

		UpdateCameraFPS(&cam, lookRotation, lean, headTimer, walkLerp);

		throwCube(isPicked, cubeVelosity, ray, throwForce, collision);
		pickCube(cam, isPicked, isCubeGrounded, cubePos, cubeSize, pickDist, ray, collision, cubeCol, cubeVelosity, gravity, cubeMass, deltaTime, winSize);

		Vector3 nextPos = Vector3Add(cam.position, playerVelosity);

		UpdateCameraPro(&cam,
			{ playerVelosity.x, playerVelosity.z, 0.0f },
			{ GetMouseDelta().x * 0.13f, GetMouseDelta().y * 0.13f }, 0);


		cam.target.y += playerVelosity.y * deltaTime;


		/*if (CheckCollisionBoxes({ { cam.position.x - 0.1f / 2.0f, cam.position.y - 2.0f / 2.0f, cam.position.z - 0.1f / 2.0f}, {cam.position.x + 0.1f / 2.0f, cam.position.y + 2.0f / 2.0f, cam.position.z + 0.1f / 2.0f,} }, { { cubePos.x - cubeSize.x / 2, cubePos.y - cubeSize.y / 2, cubePos.z - cubeSize.z / 2}, {cubePos.x + cubeSize.x / 2, cubePos.y + cubeSize.y / 2, cubePos.z + cubeSize.z / 2} }))
		{

			Vector3 surfPos = cam.position;
			cam.position = oldCamPos;

			cam.position.x = surfPos.x;
			if (CheckCollisionBoxes({ { cam.position.x - 0.1f / 2.0f, cam.position.y - 2.0f / 2.0f, cam.position.z - 0.1f / 2.0f}, {cam.position.x + 0.1f / 2.0f, cam.position.y + 2.0f / 2.0f, cam.position.z + 0.1f / 2.0f,} }, { { cubePos.x - cubeSize.x / 2, cubePos.y - cubeSize.y / 2, cubePos.z - cubeSize.z / 2}, {cubePos.x + cubeSize.x / 2, cubePos.y + cubeSize.y / 2, cubePos.z + cubeSize.z / 2} })) 
			{
				playerVelosity.x = 0;
				cam.position.x = oldCamPos.x;
			}

			cam.position.y = surfPos.y;
			if (CheckCollisionBoxes({ { cam.position.x - 0.1f / 2.0f, cam.position.y - 2.0f / 2.0f, cam.position.z - 0.1f / 2.0f}, {cam.position.x + 0.1f / 2.0f, cam.position.y + 2.0f / 2.0f, cam.position.z + 0.1f / 2.0f,} }, { { cubePos.x - cubeSize.x / 2, cubePos.y - cubeSize.y / 2, cubePos.z - cubeSize.z / 2}, {cubePos.x + cubeSize.x / 2, cubePos.y + cubeSize.y / 2, cubePos.z + cubeSize.z / 2} }))
			{
				isGrounded = 1;
				playerVelosity.y = 0;
				cam.position.y = oldCamPos.y;
			}

			cam.position.z = surfPos.z;
			if (CheckCollisionBoxes({ { cam.position.x - 0.1f / 2.0f, cam.position.y - 2.0f / 2.0f, cam.position.z - 0.1f / 2.0f}, {cam.position.x + 0.1f / 2.0f, cam.position.y + 2.0f / 2.0f, cam.position.z + 0.1f / 2.0f,} }, { { cubePos.x - cubeSize.x / 2, cubePos.y - cubeSize.y / 2, cubePos.z - cubeSize.z / 2}, {cubePos.x + cubeSize.x / 2, cubePos.y + cubeSize.y / 2, cubePos.z + cubeSize.z / 2} }))
			{
				playerVelosity.z = 0;
				cam.position.z = oldCamPos.z;
			}
		}*/


		BeginDrawing();	
		BeginMode3D(cam);
		BeginShaderMode(shader);
		ClearBackground(SKYBLUE);

		
		DrawCube(cubePos, cubeSize.x, cubeSize.y, cubeSize.z, cubeCol);

		DrawPlane({ 0,-1,0 }, { 30,30 }, DARKGRAY);

		DrawCube({ 0.0f, 1, -25.0f }, 10.0f, 4.0f, 0.1f, DARKGREEN);
		DrawCube({ 0.0f, 1,  25.0f }, 10.0f, 4.0f, 0.1f, DARKGREEN);

		DrawCube({ -25.0f, 1, 0.0f }, 0.1f, 4.0f, 10.0f, DARKGREEN);
		DrawCube({ 25.0f, 1, 0.0f }, 0.1f, 4.0f, 10.0f, DARKGREEN);

		EndShaderMode();

		EndMode3D();

		DrawFPS(10, 10);

		DrawText(".", winSize.x / 2, winSize.y / 2 - 11 , 16, WHITE);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}

void throwCube(bool& isPicked, Vector3& cubeVelosity, Ray& ray, float throwForce, RayCollision &collision)
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

static void UpdateCameraFPS(Camera* camera, Vector2 lookRotation, Vector2 lean, float headTimer, float walkLerp)
{
	const Vector3 up = { 0.0f, 1.0f, 0.0f };
	const Vector3 targetOffset = { 0.0f, 0.0f, -1.0f };

	// Left and right
	Vector3 yaw = Vector3RotateByAxisAngle(targetOffset, up, lookRotation.x);

	// Clamp view up
	float maxAngleUp = Vector3Angle(up, yaw);
	maxAngleUp -= 0.001f; // Avoid numerical errors
	if (-(lookRotation.y) > maxAngleUp) { lookRotation.y = -maxAngleUp; }

	// Clamp view down
	float maxAngleDown = Vector3Angle(Vector3Negate(up), yaw);
	maxAngleDown *= -1.0f; // Downwards angle is negative
	maxAngleDown += 0.001f; // Avoid numerical errors
	if (-(lookRotation.y) < maxAngleDown) { lookRotation.y = -maxAngleDown; }

	// Up and down
	Vector3 right = Vector3Normalize(Vector3CrossProduct(yaw, up));
	
	// Rotate view vector around right axis
	float pitchAngle = -lookRotation.y - lean.y;
	pitchAngle = Clamp(pitchAngle, -PI / 2 + 0.0001f, PI / 2 - 0.0001f); // Clamp angle so it doesn't go past straight up or straight down
	Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, pitchAngle);

	// Head animation
	// Rotate up direction around forward axis
	float headSin = sinf(headTimer * PI);
	float headCos = cosf(headTimer * PI);
	const float stepRotation = 0.01f;
	camera->up = Vector3RotateByAxisAngle(up, pitch, headSin * stepRotation + lean.x);

	// Camera BOB
	const float bobSide = 0.1f;
	const float bobUp = 0.15f;
	Vector3 bobbing = Vector3Scale(right, headSin * bobSide);
	bobbing.y = fabsf(headCos * bobUp);

	camera->position = Vector3Add(camera->position, Vector3Scale(bobbing, walkLerp));
	camera->target = Vector3Add(camera->position, pitch);
}

void UpdateBody(Body* body, float rot, char side, char forward, bool jumpPressed, bool crouchHold, float gravity, float jumpForce, float control, float airDrag, float maxSpeed, float maxAccel, float crouchSpeed, float friction)
{
	Vector2 input = { (float)side, (float)-forward };

#if defined(NORMALIZE_INPUT)
	// Slow down diagonal movement
	if ((side != 0) && (forward != 0)) input = Vector2Normalize(input);
#endif

	float delta = GetFrameTime();

	if (!body->isGrounded) body->velocity.y -= gravity * delta;

	if (body->isGrounded && jumpPressed)
	{
		body->velocity.y = jumpForce;
		body->isGrounded = false;

		// Sound can be played at this moment
		//SetSoundPitch(fxJump, 1.0f + (GetRandomValue(-100, 100)*0.001));
		//PlaySound(fxJump);
	}

	Vector3 front = { sinf(rot), 0.f, cosf(rot) };
	Vector3 right = { cosf(-rot), 0.f, sinf(-rot) };

	Vector3 desiredDir = { input.x * right.x + input.y * front.x, 0.0f, input.x * right.z + input.y * front.z, };
	body->dir = Vector3Lerp(body->dir, desiredDir, control * delta);

	float decel = (body->isGrounded ? friction : airDrag);
	Vector3 hvel = { body->velocity.x * decel, 0.0f, body->velocity.z * decel };

	float hvelLength = Vector3Length(hvel); // Magnitude
	if (hvelLength < (maxSpeed * 0.01f)) hvel = { 0 };

	// This is what creates strafing
	float speed = Vector3DotProduct(hvel, body->dir);

	// Whenever the amount of acceleration to add is clamped by the maximum acceleration constant,
	// a Player can make the speed faster by bringing the direction closer to horizontal velocity angle
	// More info here: https://youtu.be/v3zT3Z5apaM?t=165
	maxSpeed = (crouchHold ? crouchSpeed: maxSpeed);
	float accel = Clamp(maxSpeed - speed, 0.f, maxAccel * delta);
	hvel.x += body->dir.x * accel;
	hvel.z += body->dir.z * accel;

	body->velocity.x = hvel.x;
	body->velocity.z = hvel.z;

	body->position.x += body->velocity.x * delta;
	body->position.y += body->velocity.y * delta;
	body->position.z += body->velocity.z * delta;

	// Fancy collision system against the floor
	if (body->position.y <= 0.0f)
	{
		body->position.y = 0.0f;
		body->velocity.y = 0.0f;
		body->isGrounded = true; // Enable jumping
	}
}