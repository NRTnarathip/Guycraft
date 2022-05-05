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

World* World::instance = nullptr;
void World::init() {
	tickLastTime = Time::lastTime;

	Camera* mainCame = CameraManager::GetInstance().newCamera();
	CameraManager::SwitchCamera(mainCame);

	auto* player = spawnGameObject();
	player->transform.position = glm::vec3(0,85,0);
	player->addComponent<PlayerController>();
	auto* boxCollision = player->addComponent<BoxCollision>();

	auto* zombie = spawnGameObject();
	zombie->mesh = new MeshGameObject();
	zombie->mesh->setupGL();

	typedef MeshGameObject::Vertex MeshVertex;
	auto tempUV = glm::vec2(0);
	auto tempNormal = glm::vec3(0,0,0);
	std::vector<MeshVertex> tempMeshVerticesBlock = {
		MeshVertex(glm::vec3(0,1,0), tempNormal, tempUV),
		MeshVertex(glm::vec3(0,1,1), tempNormal, tempUV),
		MeshVertex(glm::vec3(1,1,1), tempNormal, tempUV),
		MeshVertex(glm::vec3(1,1,0), tempNormal, tempUV),

		MeshVertex(glm::vec3(0,0,0), tempNormal, tempUV),
		MeshVertex(glm::vec3(0,0,1), tempNormal, tempUV),
		MeshVertex(glm::vec3(1,0,1), tempNormal, tempUV),
		MeshVertex(glm::vec3(1,0,0), tempNormal, tempUV),
	};
	std::vector<unsigned int> tempMeshIndices = {
		0,1,3, 2,3,1, //face top
		5,4,6, 7,6,4, //face down
		4,0,7, 3,7,0, //face south
		6,2,5, 1,5,2, //face north
		7,3,6, 2,6,3, //face east
		5,1,4, 0,4,1, //face west
	};
	zombie->mesh->vertices = tempMeshVerticesBlock;
	zombie->mesh->indices = tempMeshIndices;
	zombie->transform.position = glm::vec3(0,85,0);
	zombie->mesh->uploadDataMeshToGPU();


	////init general component
	//for (auto gameObject : m_gameObjects) {
	//	auto comps = gameObject->getAllComponents();
	//	for (auto c : comps) {
	//		c->init();
	//	}
	//}
}
void World::tick() {
	////update general component
	//for (auto gameObject : m_gameObjects) {
	//	auto comps = gameObject->getAllComponents();
	//	for (auto c : comps) {
	//		c->tick();
	//	}
	//}
}
void World::update() {
	//update sun direction before render chunk
	/*Shader* def = Game::ref->shaders.defaultShader;
	def->Bind();
	def->SetVec3("sunDirect", lighting.sunDirect);
	def->SetFloat("sunIntensity", lighting.sunIntensity);*/
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
