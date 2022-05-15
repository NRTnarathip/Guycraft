#pragma once
#include "Scene.h"
#include <World.h>
#include <ChunkManager.h>

//main class for play game survival, creative, world
class SceneMainGame : public Scene {
public:
	bool isNeedExitToLobby = false;
	SceneMainGame(std::string name) : Scene(name) {}
	World* world = nullptr;
	ChunkManager* chManager = nullptr;
public:
	float runtime = 0.f;
	void init();
	void setupScene();
	void playGame();
	void beforeUpdate();
	void update();
	void render();
	void exit();
};