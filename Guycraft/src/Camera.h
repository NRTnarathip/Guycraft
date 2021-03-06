#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include <vector>
#include "Physics/Transform.h"
#include "Renderer/MeshGameObject.h"

class Camera {
public:
	Transform transform;
	glm::mat4 projection = glm::mat4(1.0f);
	float sensitivity = 0.25f;
	float FOVdeg = 60.f;
	float nearPlane = 0.03f;
	float farPlane = 500.0f;
	GLFWwindow* window = nullptr;
	// Camera constructor to set up initial values
	void setupCamera(GLFWwindow* window, float FOVdeg, float nearPlane, float farPlane);
	// Updates and exports the camera matrix to the Vertex Shader
	//test feature not avaliable now!!!
	void switchMode(bool isPerspective);
	void setRenderMode(int mode);

	void drawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color);
	void drawLineViaSize(glm::vec3 start, glm::vec3 end, glm::vec3 color, float size);
	MeshGameObject mesh;
};
#endif //CAMERA_H