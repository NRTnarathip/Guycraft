#include <PlayerController.h> 
#include <Game.h>
#include "glm/gtx/string_cast.hpp"
#include <GraphicSetting.h>
#include <ClientEngine.h>
#include "Input.h"
#include <CameraManager.h>
#include <GameObject.h>

PlayerController* PlayerController::instance = nullptr;

PlayerController* PlayerController::GetInstance() {
	return instance;
}
PlayerController::PlayerController() {
	instance = this;
}
void PlayerController::init() {
	Input::GetInstance().setMouseMode(0);
}
void PlayerController::start() {

}
void PlayerController::update() { //update every frame on ECS
	UpdateInputs();
}

void PlayerController::UpdateInputs()
{
	auto window = Game::GetInstance()->window->window;
	//auto &transform = getComponent<Transform>();
	auto me = getGameObject();
	glm::vec3 &posEntity = me->transform.position;
	auto* camera = CameraManager::GetCurrentCamera();
	float speed = speedMove;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = speedRun;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = speedMove;
	}

	// Handles key inputs
	speed *= (float)Time::deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		posEntity += speed * camera->Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		posEntity += speed * glm::normalize(glm::cross(camera->Orientation, camera->Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		posEntity += speed * -camera->Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		posEntity += speed * -glm::normalize(glm::cross(camera->Orientation, camera->Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		posEntity += speed * camera->Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		posEntity += speed * -camera->Up;
	}
	// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
	// and then "transforms" them into degrees 
	//auto &mouse = Game::GetInstance()->inputManager.mouse;
	glm::vec2 mouseAxis = Input::GetInstance().mouseAxis();
	float sensinitive = 20.0f;
	float rotX = (mouseAxis.y * sensinitive) * (float)Time::deltaTime;
	float rotY = (mouseAxis.x * sensinitive) * (float)Time::deltaTime;
	// Calculates upcoming vertical change in the Orientation
	glm::vec3 newOrientation = glm::rotate(camera->Orientation, 
		glm::radians(-rotX), 
		glm::normalize(glm::cross(camera->Orientation, camera->Up)));

	// Decides whether or not the next vertical Orientation is legal or not
	if (abs(glm::angle(newOrientation, camera->Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
	{
		camera->Orientation = newOrientation;
	}

	// Rotates the Orientation left and right
	camera->Orientation = glm::rotate(camera->Orientation, glm::radians(rotY), camera->Up);
	camera->Postition = posEntity;
}
