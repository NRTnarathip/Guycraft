#include "SceneManager.h"
#include <iostream>
#include "UIObject.h"
#include "Image.h"

SceneManager* SceneManager::instance = nullptr;

void SceneManager::init() {
	
}
void SceneManager::addExistScene(Scene* scene) {
	scene->index = m_scenes.size();
	m_scenes.push_back(scene);

	if (scene->index == 0) {
		changeScene(scene->index);
	}
}
void SceneManager::beforeUpdate() {
	//on new change scene
	if (m_queueChangeScene != nullptr) {
		if (m_queueChangeScene != m_currentScene) {
			m_currentScene = m_queueChangeScene;
			m_currentScene->setActive(true);
			m_currentScene->init();
		}
		else {
			printf("you change scene again \n");
		}

		m_queueChangeScene = nullptr;
	}
	m_currentScene->beforeUpdate();
}
void SceneManager::lastUpdate() {
	m_currentScene->lastUpdate();
}
void SceneManager::update() {
	m_currentScene->m_UIMenu.updateEventInput();
	m_currentScene->update();
}
Scene* SceneManager::getCurrent() {
	return m_currentScene;
}
void SceneManager::render() {
	auto sc = m_currentScene;
	sc->render();
	sc->m_UIMenu.render();
}
void SceneManager::changeScene(int index) {
	m_queueChangeScene = m_scenes[index];
}