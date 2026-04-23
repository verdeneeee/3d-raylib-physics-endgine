#include "player.h"
#include "world.h"

Player::Player(Vector3 startPos) : velocity{ 0, 0, 0 }, isGrounded(true), maxSpeed(0.5f) {
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

	cam.position.x += velocity.x * deltaTime;
	cam.position.z += velocity.z * deltaTime;

	float playerRadius = 0.3f;

	if (cam.position.x + playerRadius > 5.0f) cam.position.x = 5.0f - playerRadius;
	if (cam.position.x - playerRadius < -5.0f) cam.position.x = -5.0f + playerRadius;
	if (cam.position.z + playerRadius > 5.0f) cam.position.z = 5.0f - playerRadius;
	if (cam.position.z - playerRadius < -5.0f) cam.position.z = -5.0f + playerRadius;
}

void Player::update(World& world, float deltaTime)
{
	jump(world, deltaTime);
	movemant(world, deltaTime);
}
