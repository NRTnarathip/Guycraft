#include "Scenes/SceneMainGame.h"
#include "CameraManager.h"
#include "PlayerController.h"
#include "ClientEngine.h"
#include "Input.h"

void SceneMainGame::init() {
	//init key
	auto &input = Input::GetInstance();
	input.registerKey(GLFW_KEY_ESCAPE);

	playGame();
}
void SceneMainGame::setupScene() {
	world = new World();
	chManager = new ChunkManager();
}
void SceneMainGame::exit() {
	isNeedExitToLobby = true;

	chManager->chunkLoader.m_threadPopulate.join();
	chManager->chunkMeshBuilding.m_thread.join();

	//exist app
	Game::GetInstance()->exit();
}
void SceneMainGame::playGame() {
	setupScene();
	world->init();
	chManager->init();
}
void SceneMainGame::beforeUpdate()
{
	auto input = &Input::GetInstance();
	if (input->isKeyDown(GLFW_KEY_ESCAPE)) {
		exit();
	}
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