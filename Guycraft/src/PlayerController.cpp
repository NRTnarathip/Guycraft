#include <PlayerController.h> 
#include <Game.h>
#include "glm/gtx/string_cast.hpp"
#include <GraphicSetting.h>
#include <ClientEngine.h>
#include "Input.h"
#include <CameraManager.h>
#include <GameObject.h>
#include "Physics/Vector.h"
#include "glm/ext.hpp"

PlayerController* PlayerController::instance = nullptr;

PlayerController* PlayerController::GetInstance() {
	return instance;
}
PlayerController::PlayerController() {
	instance = this;
}
void PlayerController::init() {
	meshBlockHighlight.setupMesh();
	Input::GetInstance().setMouseMode(0);
	getGameObject()->transform.rotation = glm::vec3(0,-90.f,0);
	//setup corsur
	auto spriteCursor = ResourceManager::GetInstance()->getSprite("assets/textures/gui/cursor");
	auto sc = SceneManager::GetInstance()->getCurrent();
	auto canvas = sc->m_UIMenu.createContainer("UI Player", {0, 0}, {1920,1080});
	auto imageCursor = canvas->createImage();
	imageCursor->rect.size = { 20, 20 };
	auto compImage = imageCursor->getComponent<Image>();
	compImage->setSprite(spriteCursor);
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
	auto came = CameraManager::GetCurrentCamera();
	auto input = &Input::GetInstance();
	auto cManager = ChunkManager::GetInstance();
	auto origin = came->transform.position;
	float pitch = glm::radians(-came->transform.rotation.x);
	float yaw = glm::radians(-came->transform.rotation.y);

	auto rayDirection = glm::vec3(cos(pitch) * cos(yaw),
		sin(pitch),  cos(pitch) * sin(yaw));

	auto hit = voxelRaycast.raycast(origin, rayDirection, 10.f);
	//on dig
	meshBlockHighlight.vertices.clear();
	meshBlockHighlight.triangles.clear();

	if (hit.isHit) {
		if (input->onMouseDown(GLFW_MOUSE_BUTTON_LEFT)) {
			auto chunk = hit.chunk;
			chunk->voxels[hit.access].type = 0;//change to block air
			cManager->chunkMeshBuilding.addQueueFront(chunk);
		}

		//block highlight
		meshBlockHighlight.genMeshCube(hit.worldPos);
	}
}
void PlayerController::render() {
	if (meshBlockHighlight.triangles.size() == 0) return;

	auto shader = ResourceManager::GetInstance()->m_shaders["block_highlight"];
	shader->Bind();
	meshBlockHighlight.draw();
	shader->UnBind();
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
		speed = speedRun;
	}
	// Handles key inputs
	float cameRotateY = me->transform.rotation.y;
	auto forward = me->transform.forward();
	auto right = me->transform.right();
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
	float cameRotateX = me->transform.rotation.x;
	float rotRight = (mouseAxis.x * camera->sensitivity);
	rotRight += cameRotateY;
	if (rotRight >= 360.f or rotRight <= -360.f) {
		rotRight = 0.f;
	}
	float rotUp = (mouseAxis.y * camera->sensitivity);
	rotUp += cameRotateX;
	rotUp = glm::clamp(rotUp, -89.9f, 89.9f);

	me->transform.rotation = glm::vec3(rotUp, rotRight, 0.f);

	camera->transform.position = posEntity;
	camera->transform.rotation = me->transform.rotation;
}
