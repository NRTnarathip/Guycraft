#pragma once
#include "Chunk.h"
#include <thread>
#include <queue>
#include <SmartQueue.h>
#include <vector>
#include "MeshChunk.h"
#include "JobGenerateMesh.h"

//engine for generate mesh chunk threading
class ChunkMeshBuilding {
public:
	static ChunkMeshBuilding* m_instance;
	std::vector<std::thread> m_threads;
	SmartQueue<MeshChunkSection*> m_queueComplete;
	SmartQueue<JobGenerateMesh> m_queueJob;
	ChunkMeshBuilding() {
		m_instance = this;
	}
	static auto GetInstance() { return m_instance; }
	void startWithThread();
	void updateMainThread();//update for main thread
	void addQueue(Chunk* chunk, int index, 
		bool isPushFront, bool isDontPushIfExist = true);
	void insertQueueAtFront(std::vector<JobGenerateMesh> jobs, bool isDontPushIfExist = true);
};