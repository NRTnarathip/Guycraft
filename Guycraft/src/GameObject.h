#pragma once
#include "Physics/Transform.h"
#include <string>
#include <vector>
#include <entt/entt.hpp>
#include "Renderer/MeshGameObject.h"
#include "Renderer/shaderClass.h"
#include "Component.h"

class GameObject {
public:
	GameObject();
	Transform* parent;
	std::vector<Transform*> childs;
	std::string name;
	Transform transform;
	//component base
	std::vector<Component*> m_components;
	entt::entity m_entity;
	entt::registry* g_registry = nullptr;
	MeshGameObject* mesh = nullptr;
	Shader* shader = nullptr;

public:
	template<typename TypeComponent>
	TypeComponent* addComponent() {
		auto &newComponent = g_registry->emplace<TypeComponent>(m_entity);
		newComponent.setupComponent(this);
		m_components.push_back(&newComponent);
		return &newComponent;
	};
	template<typename TypeComponent>
	TypeComponent* getComponent() const {
		return g_registry->get<TypeComponent>(m_entity);
	};
	std::vector<Component*> getAllComponents() {
		return m_components;
	}
	Transform* getParent() const { return parent; };
	Transform* getChildIndex(int index) const;

	void setEntityID(entt::entity id) { m_entity = id; };
	entt::entity getEntity() const { return m_entity; }
};