#include "SceneManager.h"
#include <iostream>
#include "UIObject.h"
#include "Image.h"

SceneManager* SceneManager::instance = nullptr;

void SceneManager::init() {
	
}
void SceneManager::addExistScene(Scene* scene) {
	scene->index = m_scenes.size();
	if (m_scenes.empty()) {
		scene->setActive(true);
	}
	m_scenes.push_back(scene);
}
void SceneManager::beforeUpdate() {
	auto sc = getCurrent();
	sc->beforeUpdate();
}
void SceneManager::lastUpdate() {
	auto sc = getCurrent();
	sc->lastUpdate();
}
void SceneManager::update() {
	auto sc = getCurrent();
	sc->m_UIMenu.updateEventInput();
	sc->update();
}
Scene* SceneManager::getCurrent() {
	for (auto sc : m_scenes) {
		if (sc->isActive()) {
			return sc;
		}
	}
	return nullptr;
}
void SceneManager::render() {
	auto sc = getCurrent();
	sc->render();
	sc->m_UIMenu.render();
}
void SceneManager::changeScene(int index) {
	//delete object in all scene
	for (auto sc : m_scenes) {
		if (sc->index != index) {
			sc->setActive(false);
		}
	}
	auto scNow = m_scenes[index];
	scNow->setActive(true);
}