#pragma once
#include "Chunk.h"
#include <thread>
#include <queue>
#include <SmartQueue.h>
#include <vector>
#include "MeshChunk.h"

//engine for generate mesh chunk threading
class ChunkMeshBuilding {
public:
	static ChunkMeshBuilding* m_instance;
	std::thread m_thread;
	SmartQueue<MeshChunk*> m_queueComplete;
	SmartQueue<Chunk*> m_queueJob;
	ChunkMeshBuilding() {
		if (m_instance) return;
		m_instance = this;
	}
	static auto GetInstance() { return m_instance; }
	void startWithThread();
	void updateMainThread();//update for main thread
	void addQueue(Chunk* chunk);
	void genMeshChunkNeighborEdge(Chunk* c);
};