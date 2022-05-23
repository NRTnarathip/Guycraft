#include "Transform.h"
#include "Vector.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

glm::vec3 Transform::forward() {
	auto model = getModelMatrix();
	auto inverted = glm::inverse(model);
	auto forward = glm::normalize(glm::vec3(inverted[0]));
	return forward;
}
glm::vec3 Transform::right() {
	auto model = getModelMatrix();
	auto f = forward();
	return glm::normalize(glm::cross(up(), f));
}
glm::vec3 Transform::up() {
	return { 0,1,0 };
}
glm::mat4 Transform::getModelMatrix() {
	auto matTranslate = glm::mat4(1.f);
	glm::mat4 matRotate = glm::mat4(1.0f);

	float pitch = glm::radians(-rotation.x);
	float yaw = glm::radians(-rotation.y);
	float roll = glm::radians(-rotation.z);
	matRotate = glm::rotate(matRotate, pitch, { 1.0f, 0.0f, 0.0f });
	matRotate = glm::rotate(matRotate, yaw, { 0.f, 1.0f, 0.0f });
	matRotate = glm::rotate(matRotate, roll, { 0.f, 0.f, 1.0f });

	matTranslate = glm::translate(matTranslate, position);
	return matTranslate * matRotate * glm::scale(glm::mat4(1.f), scale);
}