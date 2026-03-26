#pragma once
#include "../raylib/include/raylib.h"
#include "player.h"
#include "world.h"

struct Cube
{
	Vector3 velocity;
	Vector3 size;
	Vector3 position;
	
	Ray ray;

	float mass = 5.0f;
	bool isPicked;
	bool isGrounded;
	Color color = WHITE;
	float wallDist = 5.0f;


	Cube(Vector3 position, Vector3 size);

	void Pick(const Player& player, World& world, float deltaTime);
	void Throw(const Player& player, World& world, float deltaTime);
	void Physics(World&world, float deltaTime);

private:
	RayCollision collision;
};