#include "Scenes/SceneMainGame.h"
#include "CameraManager.h"
#include "PlayerController.h"
#include "ClientEngine.h"
#include "Input.h"

void SceneMainGame::init() {
	m_UIMenu.setupEntt(&registry);

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
	printf("on exit\n");
	chManager->chunkLoader.m_threadPopulate.join();
	for (auto &th : chManager->chunkMeshBuilding.m_threads) {
		th.join();
	}

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
	/*auto input = &Input::GetInstance();
	if (input->isKeyDown(GLFW_KEY_ESCAPE)) {
		exit();
	}*/
}
void SceneMainGame::update() {
	runtime += Time::deltaTime;
	world->update();
	chManager->update();
	chManager->chunkMeshBuilding.updateMainThread();
}
void SceneMainGame::render() {
	auto res = ResourceManager::GetInstance();

	std::vector<Shader*> shadersChunk;
	shadersChunk.push_back(res->m_shaders["chunk_block_solid"]);
	shadersChunk.push_back(res->m_shaders["chunk_block_fluid"]);
	for (auto shader : shadersChunk) {
		shader->SetFloat("worldSunLevel", world->lighting.sunLevel);
	}
	world->render();
	chManager->render();
	//debug
	auto cameManager = &CameraManager::GetInstance();
	cameManager->render();

	//render without depth
	for (auto ptrFunc : m_ptrFuncRenderWithoutDepth) {
		ptrFunc();
	}
}