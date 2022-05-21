#pragma once
#include <glm/glm.hpp>

class Transform {
public:
	Transform() {
		position = glm::vec3(0);
		scale = glm::vec3(1);
		rotation = glm::vec3(0);
	}
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 forward() {
		float yaw = glm::radians(-rotation.y);
		auto forward = glm::vec3(cos(yaw), 0.f, sin(yaw));
		return forward;
	}
	glm::vec3 right() {
		auto f = forward();
		auto right = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), f));
		return right;
	}
};
