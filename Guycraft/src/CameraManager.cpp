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
	//auto& client = ClientEngine::GetInstance();
	/*int width, height;
	glfwGetWindowSize(client.window->glfwWindow, &width, &height);
	newCamera->setupCamera(client.window->glfwWindow, 90.f, 0.005f, 1000.f);*/
	return newCamera;
}
void CameraManager::uploadCameraMatrixToShader(Shader* shader) {
	auto camPosition = m_currentCamera->Postition;
	auto camOriention = m_currentCamera->Orientation;
	auto camUp = m_currentCamera->Up;
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(camPosition, camPosition + camOriention, camUp);

	// Exports the camera matrix to the Vertex Shader
	shader->Bind();
	shader->SetMat4("view", view);
	shader->UnBind();
}