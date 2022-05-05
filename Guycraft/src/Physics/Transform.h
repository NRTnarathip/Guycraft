#pragma once
#include <glm/vec3.hpp>
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
};
