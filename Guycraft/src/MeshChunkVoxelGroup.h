#pragma once
#include "MeshChunk.h"
class Chunk;

class MeshChunkVoxelGroup {
private:
	std::mutex mutexIsNeedGenMesh;
public:
	Chunk* chunk = nullptr;
	glm::vec3 pos;
	std::mutex mutex;

	//is not saft thread
	bool isNeedGenMesh = false;
	

	bool isComplete = false;
	bool isActive = false;
	void lock() {
		mutex.lock();
	}
	void unlock() {
		mutex.unlock();
	}
	void setNeedGenMesh(bool state = true) {
		mutexIsNeedGenMesh.lock();
		isNeedGenMesh = state;
		mutexIsNeedGenMesh.unlock();
	}
	bool checkIsNeedGenMesh() {
		mutexIsNeedGenMesh.lock();
		bool b = isNeedGenMesh;
		mutexIsNeedGenMesh.unlock();
		return b;
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