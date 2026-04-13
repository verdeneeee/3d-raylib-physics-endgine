#include "../raylib/include/raylib.h"
#include "cube.h"

Cube::Cube(Vector3 position, Vector3 size)
	: position(position), size(size), velocity{ 0, 0, 0 },
	isPicked(false), isGrounded(false), mass(5.0f), color(WHITE)
{
	collision.hit = false;
	collision.distance = 0;
	collision.point = { 0, 0, 0 };
	collision.normal = { 0, 0, 0 };
}

void Cube::Pick(const Player& player, World& world, float deltaTime)
{
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		if (!collision.hit)
		{
			ray = GetScreenToWorldRay({ (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2}, player.cam);

			collision = GetRayCollisionBox(ray,
				{ { position.x - size.x / 2, position.y - size.y / 2, position.z - size.z / 2},
				{position.x + size.x / 2, position.y + size.y / 2, position.z + size.z / 2} });
		}
		else collision.hit = false;
	}

	if (isPicked)
	{
		ray = GetScreenToWorldRay({ 1280 / 2, 800 / 2 }, player.cam);
		//color = LIME;

		velocity = { 0,0,0 };

		collision.distance += GetMouseWheelMove() * 0.5f;
		if (collision.distance < 2.0f) collision.distance = 2.0f;

		position.x = ray.position.x + ray.direction.x * world.pickDist;
		position.y = ray.position.y + ray.direction.y * world.pickDist;
		position.z = ray.position.z + ray.direction.z * world.pickDist;

		isGrounded = 0;
	}

	if (position.y <= -1) isGrounded = 1;

	if (collision.hit)
	{
		isPicked = 1;
		world.pickDist = collision.distance;
	}
	else isPicked = 0;
}


void Cube::Physics(World& world, float deltaTime)
{
	if (!isGrounded)
	{
		velocity.y -= (world.gravity + mass) * deltaTime;

		position.x += velocity.x * deltaTime;
		position.y += velocity.y * deltaTime;
		position.z += velocity.z * deltaTime;

		if (position.x + size.x / 2.0f > wallDist) { position.x = wallDist - size.x / 2.0f; velocity.x *= -0.2f; }
		if (position.x - size.x / 2.0f < -wallDist) { position.x = -wallDist + size.x / 2.0f; velocity.x *= -0.2f; }
		if (position.z + size.x / 2.0f > wallDist) { position.z = wallDist - size.x / 2.0f; velocity.z *= -0.2f; }
		if (position.z - size.x / 2.0f < -wallDist) { position.z = -wallDist + size.x / 2.0f; velocity.z *= -0.2f; }
	}
	if (position.y < -0.5)
	{
		position.y = -0.5;
		velocity.y = 0;
		isGrounded = 1;
	}
}


void Cube::Throw(const Player& player, World& world, float deltaTime)
{
	if (IsKeyPressed(KEY_E) && isPicked)
	{
		isPicked = 0;
		collision.hit = 0;

		velocity.x = ray.direction.x * world.throwForce;
		velocity.y = ray.direction.y * world.throwForce;
		velocity.z = ray.direction.z * world.throwForce;
	}
}
