#pragma once
#include "Scene.h"
#include <vector>
#include <iostream>

class SceneManager {
private:
	Scene* m_currentScene = nullptr;
	Scene* m_queueChangeScene = nullptr;
	static SceneManager* instance;
	std::vector<Scene*> m_scenes;
public:// Singleton
	SceneManager() { instance = this; }
	static SceneManager* GetInstance() { return instance; }
public:
	void changeScene(int index);
	void init();
	void beforeUpdate();
	void update();
	void lastUpdate();
	void render();
	Scene* getCurrent();
	template<typename T>
	T* getScene(int index) {
		return dynamic_cast<T*>(m_scenes[index]);
	};
	void addExistScene(Scene* scene);
};