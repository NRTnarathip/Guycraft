#pragma once
#include <ChunkManager.h>
#include <Component.h>
#include "Physics/VoxelRaycast.h"
#include "MeshBlockHighlight.h"

class PlayerController : public Component {
private:
	static PlayerController* instance;
	
	void updateInteractionBlock();

public:
	PlayerController();
	static PlayerController* GetInstance();
	void init();
	void start();
	void update();
	void render();
	float speedMove = 3.f;
	float speedRun = 64.f; 
	VoxelRaycast voxelRaycast;
	//highlight select block
	MeshBlockHighlight meshBlockHighlight;

	// Handles camera inputs
	void UpdateInputs();

	
};