#include "VoxelRaycast.h"
#include <iostream>
#include "glm/ext.hpp"

#define MAX(a,b) ((a > b ? a : b))
#define FRAC(x, a)
VoxelRaycast::VoxelRaycast() {
	cManager = ChunkManager::GetInstance();
}
VoxelHit VoxelRaycast::raycast(glm::vec3 rayStart, glm::vec3 direction, const int blockDistance)
{
	std::queue<VoxelHit> queueVoxelHit;

	VoxelHit hit;
	hit.isHit = false;
	auto ray = direction * blockDistance;
	auto rayEnd = rayStart + ray;
	glm::ivec3 currentVoxelPos = glm::vec3(floor(rayStart.x),
		floor(rayStart.y), floor(rayStart.z));
	glm::ivec3 endVoxelPos = glm::vec3(floor(rayEnd.x) ,
		floor(rayEnd.y), floor(rayEnd.z));

	hit.posBlockWorld = currentVoxelPos;
	hit.posHit = rayStart;

	queueVoxelHit.push(hit);
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

	glm::vec3 hitPoint = rayStart;
	int indexTraversal = 0;
	while (indexTraversal != blockDistance) {
		VoxelHit hit;
		hit.normal = glm::vec3(0);

		if (tMaxX < tMaxY && tMaxX < tMaxZ) {
			float currentXNext = direction.x > 0 ? currentVoxelPos.x + 1 : currentVoxelPos.x;
			float currentXNextTime = (currentXNext - rayStart.x) / ray.x;
			hit.posHit = ray * currentXNextTime + rayStart;
			hit.normal.x = -stepX;

			currentVoxelPos.x += stepX;
			tMaxX += tDeltaX;
		}
		else if (tMaxY < tMaxX && tMaxY < tMaxZ) {
			float currentYNext = direction.y > 0 ? currentVoxelPos.y + 1 : currentVoxelPos.y;
			float currentYNextTime = (currentYNext - rayStart.y) / ray.y;
			hit.posHit = ray * currentYNextTime + rayStart;
			hit.normal.y = -stepY;

			currentVoxelPos.y += stepY;
			tMaxY += tDeltaY;
		}
		else {
			float currentZNext = direction.z > 0 ? currentVoxelPos.z + 1 : currentVoxelPos.z;
			float currentZNextTime = (currentZNext - rayStart.z) / ray.z;
			hit.posHit = ray * currentZNextTime + rayStart;
			hit.normal.z = -stepZ;

			currentVoxelPos.z += stepZ;
			tMaxZ += tDeltaZ;
		}
		hit.posBlockWorld = currentVoxelPos;
		queueVoxelHit.push(hit);
		indexTraversal++;
	}
	while(queueVoxelHit.size() > 0) {
		auto voxelHit = queueVoxelHit.front();
		queueVoxelHit.pop();
		auto posBlockWorld = voxelHit.posBlockWorld;

		if (posBlockWorld.y < 0.f or posBlockWorld.y > CHUNK_HEIGHT_INDEX) continue;

		auto chunkPos = cManager->ToChunkPosition(posBlockWorld);
		auto chunk = cManager->getChunk({ chunkPos.x, chunkPos.z });
		if (chunk == nullptr) break;

		glm::vec3 blockPos = posBlockWorld - chunkPos;
		int chunkIndex = chunkPos.y / CHUNK_SIZE;
		uint16_t access = (int)blockPos.x + ((int)blockPos.y << 4) + ((int)blockPos.z << 8);
		auto chunkSection = chunk->m_chunks[chunkIndex];
		auto block = chunkSection->m_blocks[access];
		//hit block not air
		if (block.type > 0) {
			voxelHit.isHit = true;
			voxelHit.block = block;
			voxelHit.posChunk = chunkPos;
			voxelHit.chunkSection = chunkSection;
			voxelHit.chunkIndex = chunkIndex;
			voxelHit.posBlock = blockPos;
			voxelHit.access = access;
			return voxelHit;
		}
	}
}
