#pragma once
#include "MeshChunk.h"
class MeshChunkVoxelGroup {
public:
	std::mutex mutex;
	bool isNeedGenMesh = false;
	bool isComplete = false;
	void lock() {
		mutex.lock();
	}
	void unlock() {
		mutex.unlock();
	}
	void clear() {
		isNeedGenMesh = true;
		isComplete = false;

		fluid.clearOnGPU();
		solid.clearOnGPU();
	}
	MeshChunk fluid;
	MeshChunk solid;
};