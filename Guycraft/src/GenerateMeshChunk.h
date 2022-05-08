#pragma once
#include <thread>
#include <queue>
#include <SmartQueue.h>
#include <vector>
class GenMeshChunk {
public:
	SmartQueueChunkGroup queTransferGPU;
	std::vector<std::thread> listThreadGenMesh;
	void init();
	void update();//update from engine
};