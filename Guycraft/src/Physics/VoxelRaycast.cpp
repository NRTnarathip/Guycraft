#include "VoxelRaycast.h"
#include <iostream>
#include "glm/ext.hpp"

#define MAX(a,b) ((a > b ? a : b))
#define FRAC(x, a)
VoxelRaycast::VoxelRaycast() {
	cManager = ChunkManager::GetInstance();
}
VoxelHit VoxelRaycast::raycast(glm::vec3 rayStart, glm::vec3 direction, float distance)
{
	std::queue<glm::vec3> queueTravelVoxel;
	VoxelHit hit;
	hit.isHit = false;
	auto ray = direction * distance;
	auto rayEnd = rayStart + ray;
	glm::ivec3 currentVoxelPos = glm::vec3(floor(rayStart.x),
		floor(rayStart.y), floor(rayStart.z));
	glm::ivec3 endVoxelPos = glm::vec3(floor(rayEnd.x) ,
		floor(rayEnd.y), floor(rayEnd.z));

	queueTravelVoxel.push(currentVoxelPos);
	int stepX = 0, stepY = 0, stepZ = 0;
	float tMaxX = FLT_MAX, tMaxY = FLT_MAX, tMaxZ = FLT_MAX;
	float tDeltaX = FLT_MAX, tDeltaY = FLT_MAX, tDeltaZ = FLT_MAX;
	//tile x
	if (direction.x > 0.f) {
		stepX = 1;
		tMaxX = (currentVoxelPos.x + 1 - rayStart.x) / direction.x;
	}
	else if (direction.x < 0.f) {
		stepX = -1;
		tMaxX = (currentVoxelPos.x - rayStart.x) / direction.x;
	}
	//tile y
	if (direction.y > 0.f) {
		stepY = 1;
		tMaxY = (currentVoxelPos.y + 1 - rayStart.y) / direction.y;
	}
	else if (direction.y < 0.f) {
		stepY = -1;
		tMaxY = (currentVoxelPos.y - rayStart.y) / direction.y;
	}
	//tile z
	if (direction.z > 0.f) {
		stepZ = 1;
		tMaxZ = (currentVoxelPos.z + 1 - rayStart.z) / direction.z;
	}
	else if (direction.z < 0.f) {
		stepZ = -1;
		tMaxZ = (currentVoxelPos.z  - rayStart.z) / direction.z;
	}

	tDeltaX = stepX == 0 ? FLT_MAX : 1.f / (direction.x * stepX);
	tDeltaY = stepY == 0 ? FLT_MAX : 1.f / (direction.y * stepY);
	tDeltaZ = stepZ == 0 ? FLT_MAX : 1.f / (direction.z * stepZ);

	int loop = 0;
	while (currentVoxelPos != endVoxelPos) {
		if (loop > 30) {
			//printf("errro voxel traversal\n");
			break;
		}
		loop++;

		if (tMaxX < tMaxY && tMaxX < tMaxZ) {
			currentVoxelPos.x += stepX;
			tMaxX += tDeltaX;
		}
		else if (tMaxY < tMaxX && tMaxY < tMaxZ) {
			currentVoxelPos.y += stepY;
			tMaxY += tDeltaY;
		}
		else {
			currentVoxelPos.z += stepZ;
			tMaxZ += tDeltaZ;
		}
		queueTravelVoxel.push(currentVoxelPos);
	}
	while(queueTravelVoxel.size() > 0) {
		auto voxelPosTravel = queueTravelVoxel.front();
		queueTravelVoxel.pop();
		if (voxelPosTravel.y < 0.f or voxelPosTravel.y > CHUNK_HEIGHT_INDEX) continue;

		auto chunkPos = cManager->ToChunkPosition(voxelPosTravel);
		auto chunk = cManager->getChunk({ chunkPos.x, chunkPos.z });
		if (chunk == nullptr) break;

		glm::vec3 blockPos = voxelPosTravel - chunkPos;
		int chunkIndex = chunkPos.y / CHUNK_SIZE;
		uint16_t access = (int)blockPos.x + ((int)blockPos.y << 4) + ((int)blockPos.z << 8);
		auto chunkSection = chunk->m_chunks[chunkIndex];
		auto block = chunkSection->m_blocks[access];
		//hit block not air
		if (block.type > 0) {
			hit.isHit = true;
			hit.block = block;
			hit.chunkSection = chunkSection;
			hit.worldPos = voxelPosTravel;
			hit.chunkIndex = chunkIndex;
			hit.blockPos = blockPos;
			hit.access = access;
			break;
		}
	}
	return hit;
}
