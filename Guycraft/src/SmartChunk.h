#pragma once
#include <ChunkGroup.h>
#include <mutex>
class SmartChunkGroup {
private:
	std::mutex mutex;
	bool isEmty = false;
	ChunkGroup* cg = NULL; //dont set. cg has manager on class Chunk Manager Pooling
public:
	SmartChunkGroup(glm::ivec2 posNew) {
		cg = new ChunkGroup(posNew);
	}
	
	ChunkGroup* get(){
		return cg;
	}
	//dont forget use mutex 
	bool chunkIsEmty() {
		return isEmty;
	}
	void onSetup() {
		isEmty = false;
		//should clear data mesh and block

	}
	void onDelete() {
		isEmty = true;
	}
	SmartChunkGroup(ChunkGroup* cg) {
		this->cg = cg;
	}
	void lock() {
		mutex.lock();
	}
	void unlock() {
		mutex.unlock();
	}
};