#pragma once
#include "Scene.h"
#include <World.h>
#include <ChunkManager.h>

class SceneMainGame : public Scene {
public:
	SceneMainGame(std::string name) : Scene(name) {}
	bool isNeedExitToLobby = false;
	World* world = nullptr;
	ChunkManager* chManager = nullptr;
	float runtime = 0.f;

	void init();
	void setupScene();
	void playGame();
	void beforeUpdate();
	void update();
	void render();
	void exit();
};
