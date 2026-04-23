#pragma once
#include "world.h"

struct Player
{
	Camera3D cam;
	Vector3 velocity;

	bool isGrounded;
	float maxSpeed = 5.0f;

	Player(Vector3 startPos);

	void jump(const World& world, float deltaTime);
	void movemant(World& world, float deltaTime);
	void update(World& world, float deltaTime);
};
