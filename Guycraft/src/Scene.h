#pragma once
#include <string>
#include "entt/entt.hpp"
#include <iostream>
#include "GameObject.h"
#include "UIMenu.h"

class Scene {
private:
	bool isHasInitial = false;
	bool m_active = false;
public:
	Scene();
	Scene(std::string sceneNam);
	std::string name = "Unknow Scene";
	unsigned int index = 0;

	std::vector<void(*)()> m_ptrFuncRenderWithoutDepth;

	void setActive(bool state) {
		m_active = state;
	};
	bool isActive() const { return m_active; }
	void registerRenderWithoutDepth(void (*refFunc)());
	virtual void init() {
		if (isHasInitial) return;

		isHasInitial = true;
	};
	virtual void exit() {};
	virtual void beforeUpdate() {};
	virtual void update() {};
	virtual void lastUpdate() {};
	virtual void render() {};
	virtual void renderWithoutDepth() {};
public:
	UIMenu m_UIMenu;
	entt::registry registry;
};