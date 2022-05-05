#pragma once
#include <ChunkManager.h>
#include <thread>
#include <queue>
#include <SmartQueue.h>
#include <vector>

class GenMeshChunk {
public:
	SmartQueueChunkGroup queTransferGPU;
	std::vector<std::thread> listThreadGenMesh;
	ChunkManager* chManager = NULL;
	void init(ChunkManager* chunkManager);
	void update();//update from engine
};