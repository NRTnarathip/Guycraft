#pragma once
#include "MeshChunk.h"
class ChunkSection;
enum StateGenerateMesh : int {
	Empty = 0,
	OnNeedGenerate = 1,
	OnGenerate = 2
};
class MeshChunkSection {
private:
	std::mutex mutexIsNeedGenMesh;
public:
	
	ChunkSection* m_chunk = nullptr;
	uint8_t m_index;
	std::mutex mutex;
	bool isComplete = false;
	bool isActive = false;
	StateGenerateMesh stateGenerateMesh = Empty;
	void lock() {
		mutex.lock();
	}
	void unlock() {
		mutex.unlock();
	}
	void clear() {
		isActive = false;
		isComplete = false;
		fluid.clearOnGPU();
		solid.clearOnGPU();
	}
	MeshChunk fluid;
	MeshChunk solid;
};