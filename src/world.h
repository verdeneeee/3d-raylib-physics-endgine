#pragma once
#include <vector>

struct World
{
	float friction = 0.86f;
	float gravity = 32.0f;
	float jumpForce = 12.0f;
	const int throwForce = 13;
	float pickDist = 2.5f;
	float accelerate = 1.5f;
	float wallDist = 5.0f;

	void drawWorld(std::vector<Vector3> walls);
};
