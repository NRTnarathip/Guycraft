//
// Created by sven on 01.05.19. Class taken and adjusted from learnopengl.com (Joey DeVries)
//

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
};
