#pragma once
#include "RectTransform.h"
#include "UIComponent.h"
#include "entt/entt.hpp"
#include <string>
#include <vector>


class UIObject {
public:
	UIObject() {
		rect.m_uiObject = this;
	}
	std::string name = "Empty UI Object";
	entt::entity m_entity;
	entt::registry* g_registry = nullptr;
	RectTransform rect;
	std::vector<UIComponent*> m_components;

	template<typename TypeComponent>
	TypeComponent* addComponent() {
		auto& newComponent = g_registry->emplace<TypeComponent>(m_entity);
		newComponent.m_uiObject = this;
		newComponent.init();
		m_components.push_back(&newComponent);
		return &newComponent;
	};

	template<typename TypeComponent>
	TypeComponent* addComponent(TypeComponent addComp) {
		auto& newComponent = g_registry->emplace<TypeComponent>(m_entity, addComp);
		newComponent.m_uiObject = this;
		newComponent.init();
		m_components.push_back(&newComponent);
		return &newComponent;
	};

	template<typename TypeComponent>
	TypeComponent* getComponent() const {
		return &g_registry->get<TypeComponent>(m_entity);
	};
	void attachObject(UIObject* child);
	void swapOrderComponent(UIComponent* from, UIComponent* to);
	void update();
	void render();
};