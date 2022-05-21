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
	//move left,right,forward,backward, camera
	UpdateInputs();


	//for interaction world
	//block
	updateInteractionBlock();
}
void PlayerController::updateInteractionBlock() {
	auto input = &Input::GetInstance();
	int maxLenghtHitBlock = 5;//from my self 0 to max next 4 block
	auto came = CameraManager::GetCurrentCamera();
	//on hit block
	if (input->onMouseDown(GLFW_MOUSE_BUTTON_LEFT)) {

		auto currentRay = glm::vec3(0.f,0.f,0.2f);
		auto rayDirection = glm::vec3(0.f,0.2f,1.);

		for (int i = 0; i < maxLenghtHitBlock; i++) {
			currentRay = rayDirection + currentRay;
			printf("step %d pos to voxel x:%f y%f z:%f\n",i, currentRay.x,
				currentRay.y, currentRay.z);
		}
	}
}
void PlayerController::UpdateInputs()
{
	auto input = &Input::GetInstance();
	auto window = Game::GetInstance()->window->window;
	//auto &transform = getComponent<Transform>();
	auto me = getGameObject();
	glm::vec3 &posEntity = me->transform.position;
	auto* camera = CameraManager::GetCurrentCamera();
	float speed = speedMove;
	if (input->isKey(GLFW_KEY_LEFT_SHIFT))
	{
		speed = speedRun * 3;
	}
	// Handles key inputs
	float cameRotateY = camera->transform.rotation.y;
	auto forward = camera->transform.forward();
	auto right = camera->transform.right();

	speed *= (float)Time::deltaTime;
	if (input->isKey(GLFW_KEY_W)) {
		posEntity += speed * forward;
	}
	else if (input->isKey(GLFW_KEY_S)) {
		posEntity -= speed * forward;
	}
	if (input->isKey(GLFW_KEY_D)) {
		posEntity += speed * right;
	}
	else if (input->isKey(GLFW_KEY_A)) {
		posEntity -= speed * right;
	}

	if (input->isKey(GLFW_KEY_SPACE))
	{
		posEntity.y += speed;
	}
	else if (input->isKey(GLFW_KEY_LEFT_CONTROL))
	{
		posEntity.y -= speed;
	}

	glm::vec2 mouseAxis = Input::GetInstance().mouseAxis();
	float cameRotateX = camera->transform.rotation.x;
	float rotRight = (mouseAxis.x * camera->sensitivity);
	rotRight += cameRotateY;
	if (rotRight >= 360.f or rotRight <= -360.f) {
		rotRight = 0.f;
	}
	float rotUp = (mouseAxis.y * camera->sensitivity);
	rotUp += cameRotateX;
	rotUp = glm::clamp(rotUp, -89.9f, 89.9f);

	camera->transform.position = posEntity;
	camera->transform.rotation.x = rotUp;
	camera->transform.rotation.y = rotRight;
}
