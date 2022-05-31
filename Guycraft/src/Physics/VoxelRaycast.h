#pragma once
#include <ChunkManager.h>
struct VoxelHit {
	bool isHit;
	Block block;
	glm::ivec3 blockPos;
	int chunkIndex;
	glm::vec3 worldPos;
	ChunkSection* chunkSection;
	uint16_t access;//index of voxel in chunk section
};
class VoxelRaycast {
public:
	VoxelRaycast();
	VoxelHit raycast(glm::vec3 rayStart, glm::vec3 rayDirection, float distance);
private:
	ChunkManager* cManager = nullptr;
};