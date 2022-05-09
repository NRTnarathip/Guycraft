#include "Scenes/SceneMainGame.h"
#include "CameraManager.h"
#include "PlayerController.h"
#include "ClientEngine.h"

void SceneMainGame::setupScene() {
	world = new World();
	chManager = new ChunkManager();
}
void SceneMainGame::playGame() {
	setupScene();
	world->init();
	chManager->init();
}
void SceneMainGame::update() {
	runtime += Time::deltaTime;
	world->update();
	chManager->update();
	chManager->chunkMeshBuilding.updateMainThread();
}
void SceneMainGame::render() {
	world->render();
	chManager->render();
}