#pragma once
#include <glm/vec3.hpp>
#include "entt/entt.hpp"
#include "Component.h"

class BoxCollision : public Component {
public:
	BoxCollision() {
		m_position = glm::vec3(0,0,0);
		m_dimension = glm::vec3(1,1,1);
		m_dimensionHalf = glm::vec3(.5f, .5f, .5f);
	};
	BoxCollision(glm::vec3 position, glm::vec3 dimension) {
		m_position = position;
		m_dimension = dimension;
		m_dimensionHalf = m_dimension / glm::vec3(2, 2, 2);
	}
	bool isCollider(BoxCollision other);
	void beforeUpdate();
	void draw();
	void update();
private:
	glm::vec3 m_dimensionHalf;
	glm::vec3 m_position;
	glm::vec3 m_dimension;
};