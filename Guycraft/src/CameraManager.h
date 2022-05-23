#pragma once
#include <Camera.h>
#include <list>
#include "Renderer/shaderClass.h"

class CameraManager {
private:
	static CameraManager* instance;
	Camera* m_currentCamera = nullptr;

	std::list<Camera*> m_cameras;
public:
	CameraManager();
	//Singletons should not be cloneable.
	CameraManager(CameraManager& other) = delete;
	//Singletons should not be assignable.
	void operator=(const CameraManager&) = delete;

	static CameraManager& GetInstance() { return *instance; };

	void uploadCameraMatrixToShader(Shader* shader);
	Camera* getCurrentCamera();
	void switchCamera(Camera* other);
	Camera* newCamera();
	void render();
public:
	//function global
	static Camera* GetCurrentCamera() { return instance->m_currentCamera; }
	static void SwitchCamera(Camera* other) { instance->switchCamera(other); }
};