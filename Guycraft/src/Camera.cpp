#include <Camera.h>
#include <Game.h>
#include <Renderer/shaderClass.h>
#include "glm/gtx/string_cast.hpp"
#include "ClientEngine.h"

void Camera::setupCamera(GLFWwindow* window, float FOVdeg, float nearPlane, float farPlane) {
	
	this->FOVdeg = FOVdeg;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	this->window = window;
	//		 ______Y+_Z-_______
	//		/	   | /       /
	//	   /       |/       /
	//	  X+-------/-------X-
	//   /        /|      /
	//	/	     / |     / 
	// /        /Y-|    /
	// /-------Z+--------
	//default coordinate system opengl it right hand
	// x+ on left
	// x- on right                    
	// z+ on near                    
	// z- on far
	// y+ on top
	// y- on down
	// Convert Coordinate system to left hand
	//change Invert axis X,Z From Left To Right In Matrix Projection;
	//set up projection one on init
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / (float)height, nearPlane, farPlane);
	projection[0][0] *= -1;//invert project axis X

	mesh.setupGL();
};
void Camera::switchMode(bool isPerspective) {
	if (isPerspective) {
		
	}
	else {

	}
}

void Camera::setRenderMode(int mode)
{
	//normal
	if (mode == 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//wireframe
	else if (mode == 1) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
}

void Camera::drawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color)
{
	drawLineViaSize(start, end, color, 1.f);
}

void Camera::drawLineViaSize(glm::vec3 start, glm::vec3 end, glm::vec3 color, float size)
{
	MeshGameObject::Vertex vertStart;
	MeshGameObject::Vertex vertEnd;
	vertStart.position = start;
	vertStart.normal = color;
	vertEnd.position = end;
	vertEnd.normal = color;
	int vertCount = mesh.vertices.size();

	mesh.vertices.push_back(vertStart);
	mesh.vertices.push_back(vertEnd);

	mesh.indices.push_back(vertCount );
	mesh.indices.push_back(vertCount + 1);

	mesh.uploadDataMeshToGPU();
}
