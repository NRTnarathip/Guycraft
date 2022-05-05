#pragma once
#include "Scene.h"
#include <World.h>

//main class for play game survival, creative, world
class SceneMainGame : public Scene {
public:
	SceneMainGame(std::string name) : Scene(name) {}
	World* world = nullptr;
public:
	float runtime = 0.f;
	void setupScene();
	void playGame();
	void update();
	void render();
};