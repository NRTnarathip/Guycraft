#pragma once
#include <glm/vec2.hpp>
#include <map>
#include <string>
#include <vector>
#include "Button.h"
#include "UIContainer.h"
#include "entt/entt.hpp"

class UIMenu {
public:
	UIMenu() {}
	entt::registry* m_registry = nullptr;
	std::map<std::string, UIContainer*> m_containers;
	UIContainer* createContainer(std::string containerName, glm::vec2 pos, glm::vec2 size);
	void setupEntt(entt::registry* refRegistry);

	void updateEventInput();
	void render();
};