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
class Camera {
public:
	//project view use coordinate system left hand
	// Stores the main vectors of the camera
	//look at forward axis +z
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, 1.0f); // rotation object
	glm::vec3 Postition = glm::vec3(0.0f, 0.0f, 0.0f); // should += offset player
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	// Adjust the speed of the camera and it's sensitivity when looking around
	float sensitivity = 20.0f;

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
