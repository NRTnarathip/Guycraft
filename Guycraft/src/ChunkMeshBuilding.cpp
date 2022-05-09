#include "ChunkMeshBuilding.h"
#include "ChunkManager.h"

ChunkMeshBuilding* ChunkMeshBuilding::m_instance = nullptr;

void useThreadChunkMeshBuilding() {
	auto chManager = ChunkManager::GetInstance();
	auto queueJob = &chManager->chunkMeshBuilding.m_queueJob;
	auto queueComplate = &chManager->chunkMeshBuilding.m_queueComplete;
	while (true) {
		if (chManager->queDeleteChunk.size() > 0) continue;
		//we should wait delete chunk all successfully 
		//before run this.
		if (queueJob->empty()) continue;

		auto chunk = queueJob->getFront();
		if (chunk->isNeedGenerateMesh == false) continue;
		chunk->generateMeshChunk();
		queueComplate->push(chunk);
	}
}
void ChunkMeshBuilding::startWithThread()
{
	std::thread thGenMesh(useThreadChunkMeshBuilding);
	m_thread = std::move(thGenMesh);
}

void ChunkMeshBuilding::updateMainThread()
{
	int maxQuePerUpdate = 5;
	for (int i = 0; i < maxQuePerUpdate; i++) {
		if (m_queueComplete.empty()) continue;
		auto chunk = m_queueComplete.getFront();
		if (chunk->isNeedGenerateMesh or chunk->isGenerateMesh or chunk->isNeedRegenerateMesh) {
			continue;
		}

		chunk->mesh.transferToGPU();
	}
}

void ChunkMeshBuilding::addQueue(Chunk* chunk)
{
	m_queueJob.push(chunk);
}