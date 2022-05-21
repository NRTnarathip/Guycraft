#include <CameraManager.h>
#include <ClientEngine.h>

CameraManager *CameraManager::instance = new CameraManager();
CameraManager::CameraManager() {

}
Camera* CameraManager::getCurrentCamera() {
	return m_currentCamera;
}
void CameraManager::switchCamera(Camera* other) {
	m_currentCamera = other;
}
Camera* CameraManager::newCamera() {
	auto* newCamera = new Camera();
	if (m_cameras.empty()) {
		switchCamera(newCamera);
	}
	m_cameras.push_back(newCamera);
	return newCamera;
}
void CameraManager::uploadCameraMatrixToShader(Shader* shader) {
	// Initializes matrices since otherwise they will be the null matrix
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	auto pos = m_currentCamera->transform.position;
	auto pitch = -m_currentCamera->transform.rotation.x;
	auto yaw = -m_currentCamera->transform.rotation.y;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	glm::vec3 cameraRight = glm::normalize(glm::cross(up, direction));
	glm::vec3 cameraUp = glm::cross(direction, cameraRight);

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(pos,pos + direction, cameraUp);

	// Exports the camera matrix to the Vertex Shader
	shader->Bind();
	shader->SetMat4("view", view);
	shader->SetMat4("projection", m_currentCamera->projection);
	shader->UnBind();
}