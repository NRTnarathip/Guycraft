#include <World.h>
#include <Game.h>
#include "Physics/Transform.h"
#include <CameraManager.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <GameObject.h>
#include "PlayerController.h"
#include "Physics/BoxCollision.h"
#include "Renderer/MeshGameObject.h"
#include "ClientEngine.h"

World* World::instance = nullptr;
void World::init() {
	tickLastTime = 0;
	Camera* mainCame = CameraManager::GetInstance().newCamera();
	mainCame->setupCamera(ClientEngine::GetInstance().window->window,90.f, 0.001f, 1000.f);
	CameraManager::SwitchCamera(mainCame);

	auto* player = spawnGameObject();
	player->addComponent<PlayerController>();
	auto* boxCollision = player->addComponent<BoxCollision>();
}
void World::tick() {
	lighting.sunLevel = floor(((sin(tickLastTime) + 1) / 2) * 16);
	printf("sun level %d\n",lighting.sunLevel);

	for (auto gameObject : m_gameObjects) {
		auto comps = gameObject->getAllComponents();
		for (auto c : comps) {
			c->tick();
		}
	}
}
void World::update() {
	//update sun direction before render chunk
	float timeNow = Time::lastTime;
	if (timeNow - tickLastTime > tickUseTime) {
		tickLastTime = timeNow;
		tickCountter++;
		tick();
	}
	//update general component
	for (auto gameObject : m_gameObjects) {
		auto comps = gameObject->getAllComponents();
		for (auto c : comps) {
			c->update();
		}
	}
}
void World::render() {
	//render mesh game object
	auto shader = ResourceManager::GetInstance()->m_shaders["gameobject"];
	CameraManager::GetInstance().uploadCameraMatrixToShader(shader);
	shader->Bind();
	for (GameObject* obj : m_gameObjects) {
		auto comps = obj->getAllComponents();
		for (auto c : comps) {
			c->render();
		}

		if (obj->mesh == nullptr) { continue; }

		auto mesh = obj->mesh;
		//upload matrix game object to shader
		glm::vec3 pos = obj->transform.position;

		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, pos);
		shader->SetMat4("model", model);
		mesh->render();
	}
	shader->UnBind();
}
GameObject* World::spawnGameObject() {
	entt::entity newEntity = m_registry.create();
	auto* newGameobject = new GameObject();
	newGameobject->setEntityID(newEntity);
	newGameobject->name ="New GameObject(" + std::to_string((uint32_t)newEntity) + ")";
	newGameobject->shader = ResourceManager::GetInstance()->m_shaders["gameobject"];
	m_gameObjects.push_back(newGameobject);
	return newGameobject;
}
