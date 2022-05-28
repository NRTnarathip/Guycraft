#pragma once
#include <ChunkManager.h>
struct VoxelHit {
	bool isHit;
	Voxel voxel;
	glm::ivec3 pos;
	int voxelGroup;
	glm::vec3 worldPos;
	Chunk* chunk;
	u32 access;//index of voxel in chunk
};
class VoxelRaycast {
public:
	VoxelRaycast();
	VoxelHit raycast(glm::vec3 rayStart, glm::vec3 rayDirection, float distance);

private:
	ChunkManager* cManager = nullptr;
};