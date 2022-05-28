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
	std::thread m_thread;
	SmartQueue<MeshChunkVoxelGroup*> m_queueComplete;
	SmartQueue<JobGenerateMesh> m_queueJob;
	ChunkMeshBuilding() {
		m_instance = this;
	}
	static auto GetInstance() { return m_instance; }
	void startWithThread();
	void updateMainThread();//update for main thread
	void addQueue(Chunk* chunk, int voxelGroup, bool isPushFront,bool isGenForNeighbor = false);
};