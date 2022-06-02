#pragma once
#include <ChunkManager.h>
struct VoxelHit {
	bool isHit;
	Block block;
	glm::vec3 posHit;
	glm::vec3 normal;
	glm::ivec3 posBlock;
	glm::vec3 posChunk;
	glm::vec3 posBlockWorld;
	int chunkIndex;
	ChunkSection* chunkSection;
	uint16_t access; //index of voxel in chunk section
};
class VoxelRaycast {
public:
	VoxelRaycast();
	VoxelHit raycast(glm::vec3 rayStart, glm::vec3 rayDirection,const int Blockdistance);
private:
	ChunkManager* cManager = nullptr;
};