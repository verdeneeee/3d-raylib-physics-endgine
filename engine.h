#pragma once
#include "dependensis.h"

class Game
{
public:
	Game();
	~Game();
	void run();
private:
	void Update();
	void Draw();
	
	World world;
	Player player;
	Cube cube;
	std::vector <Vector3> walls;

	RenderTexture2D target;
	RenderTexture2D fxaaTexture;
	Shader shaders[6];
	Shader fxaaShader;
	int currentShader = 0;
	int shaderCount = 6;
	int resLoc;
};