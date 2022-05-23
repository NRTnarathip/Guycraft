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
	glm::vec3 right();
	glm::vec3 forward();
	glm::vec3 up();
	glm::mat4 getModelMatrix();
};
