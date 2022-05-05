#include "Scenes/SceneMainGame.h"
#include "CameraManager.h"
#include "PlayerController.h"
#include "ClientEngine.h"

void SceneMainGame::setupScene() {

}
void SceneMainGame::playGame() {
	world = new World();
	world->init();
}
void SceneMainGame::update() {
	runtime += Time::deltaTime;
	world->update();
}
void SceneMainGame::render() {
	world->render();
}