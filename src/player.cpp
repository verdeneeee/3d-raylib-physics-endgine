#include "player.h"
#include "world.h"

Vector3 Vector3Subtract(Vector3 vec1, Vector3 vec2) 
{
    return { vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z };
}

Vector3 Vector3Add(Vector3 vec1, Vector3 vec2)
{
    return { vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z };
}

Vector3 Vector3Scale(Vector3 vec, float scale) 
{
    return { vec.x * scale, vec.y * scale, vec.z * scale };
}

float Vector3Length(Vector3 vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

Vector3 Vector3Normalize(Vector3 vec)
{
    float length = Vector3Length(vec);
    if (length > 0)
    {
        return { vec.x / length, vec.y / length, vec.z / length };
    }
    return { 0, 0, 0 };
}

Player::Player(Vector3 startPos) : velocity{ 0, 0, 0 }, isGrounded(true), maxSpeed(0.5f)
{
	cam.fovy = 90;
	cam.position = startPos;
	cam.target = { 0, 1, 0 };
	cam.projection = CAMERA_PERSPECTIVE;
	cam.up = { 0, 1, 0 };
}

void Player::jump(const World& world, float deltaTime)
{
	if (IsKeyDown(KEY_SPACE) && isGrounded)
	{
		isGrounded = 0;
		velocity.y = world.jumpForce;
	}

	velocity.y -= world.gravity * deltaTime;
	cam.position.y += velocity.y * deltaTime;

	if (cam.position.y <= 0)
	{
		cam.position.y = 0;
		velocity.y = 0.0f;
		isGrounded = true;
	}
}


void Player::movemant(World& world, float deltaTime)
{
    UpdateCameraPro(&cam,
        { velocity.x, velocity.z, 0.0f },
        { GetMouseDelta().x * 0.13f, GetMouseDelta().y * 0.13f }, 0);

    cam.target.y += velocity.y * deltaTime;

    if (IsKeyDown(KEY_W)) velocity.x += world.accelerate * deltaTime;
    if (IsKeyDown(KEY_S)) velocity.x -= world.accelerate * deltaTime;
    if (IsKeyDown(KEY_D)) velocity.z += world.accelerate * deltaTime;
    if (IsKeyDown(KEY_A)) velocity.z -= world.accelerate * deltaTime;

    velocity.x *= world.friction;
    velocity.z *= world.friction;

    if (velocity.x > maxSpeed) velocity.x = maxSpeed;
    if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;
    if (velocity.z > maxSpeed) velocity.z = maxSpeed;
    if (velocity.z < -maxSpeed) velocity.z = -maxSpeed;

    Vector3 forward = Vector3Subtract(cam.target, cam.position);

    cam.position.x += velocity.x * deltaTime;
    cam.position.z += velocity.z * deltaTime;

    float playerRadius = 0.3f;
    bool collided = false;

    if (cam.position.x + playerRadius > world.wallDist)
    {
        cam.position.x = world.wallDist - playerRadius;
        collided = true;
    }

    if (cam.position.x - playerRadius < -world.wallDist) 
    {
        cam.position.x = -world.wallDist + playerRadius;
        collided = true;
    }

    if (cam.position.z + playerRadius > world.wallDist) 
    {
        cam.position.z = world.wallDist - playerRadius;
        collided = true;
    }

    if (cam.position.z - playerRadius < -world.wallDist) 
    {
        cam.position.z = -world.wallDist + playerRadius;
        collided = true;
    }

    if (collided) 
    {
        float length = Vector3Length(forward);
        if (length > 0) 
        {
            forward = Vector3Normalize(forward);
            forward = Vector3Scale(forward, length);
            cam.target = Vector3Add(cam.position, forward);
        }
    }
}

void Player::update(World& world, float deltaTime)
{
	jump(world, deltaTime);
	movemant(world, deltaTime);
}
