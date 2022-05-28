#pragma once
#include "MeshChunk.h"
class Chunk;

class MeshChunkVoxelGroup {
public:
	Chunk* chunk = nullptr;
	glm::vec3 pos;
	std::mutex mutex;
	bool isNeedGenMesh = false;
	bool isComplete = false;
	bool isActive = false;
	void lock() {
		mutex.lock();
	}
	void unlock() {
		mutex.unlock();
	}
	void clear() {
		isActive = false;
		isNeedGenMesh = true;
		isComplete = false;

		fluid.clearOnGPU();
		solid.clearOnGPU();
	}
	MeshChunk fluid;
	MeshChunk solid;
};