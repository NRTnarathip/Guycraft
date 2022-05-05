#include "UIMenu.h"
#include <glm/glm.hpp>
#include "ResourceManager.h"
#include "GUI.h"

UIContainer* UIMenu::createContainer(std::string containerName, glm::vec2 pos, glm::vec2 size) {
	auto newContainer = new UIContainer(containerName, pos, size);
	newContainer->m_registry = m_registry;
	m_containers.emplace(containerName, newContainer);
	return newContainer;
}
void UIMenu::setupEntt(entt::registry* registry) {
	m_registry = registry;
}
void UIMenu::updateEventInput() {
	//update polling event mouse button
	for (auto elemContent : m_containers) {
		auto content = elemContent.second;
		for (auto button : content->m_buttons) {
			button->updateEventInput();
		}
	}
}
void UIMenu::render() {
	GUI* gui = &GUI::GetInstance();

	//gl renderer for 2D;
	glDisable(GL_DEPTH_TEST);
	auto res = ResourceManager::GetInstance();
	auto shaderSprite = res->m_shaders["sprite"];
	auto shText = res->m_shaders["text"];
	shaderSprite->Bind();
	shaderSprite->SetMat4("projection", gui->projection);
	shText->Bind();
	shText->SetMat4("projection", gui->projection);
	for (auto elemContain : m_containers) {
		auto container = elemContain.second;
		for (auto rectUIObject : container->rect.childs) {
			rectUIObject->m_uiObject->render();
		}
	}
	//gui renderer for 3D;
	glEnable(GL_DEPTH_TEST);
}