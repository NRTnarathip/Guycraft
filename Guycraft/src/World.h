#pragma once
#ifndef WORLD_H
#define WORLD_H
#include <ChunkManager.h>
#include "entt/entt.hpp"
#include "GameObject.h"

class Lighting {
public:
	glm::vec3 sunDirect = glm::vec3(0.f, -1.f, 0.f);
	float sunIntensity = 1.0f;
	uint8_t sunLevel = 15;
};
class World {
private:
	static World* instance;
	//update 20 tick per 1 second;
	//1 tick use 1000/20;s, 50 second per tick
	int tickRate = 20;
	// 0.025 second
	const float tickUseTime = 1.f / tickRate;
	unsigned int tickCountter = 0;
	
	float tickLastTime = 0.f;
	float totaltime = 0.f;

	//entity system component
	entt::registry m_registry;
	std::vector<GameObject*> m_gameObjects;
public:
	World() { instance = this; }
	static World* GetInstance() { return instance; }
	static entt::registry& GetRegistry() { return instance->m_registry; }
	//base func
	void init();
	void tick();
	void update();//update every frame
	void render(); //render all game object;

	//entity system
	entt::registry& getRegistry() { return m_registry; }
	GameObject* spawnGameObject();
	Lighting lighting;

};
#endif // !WORLD_H
