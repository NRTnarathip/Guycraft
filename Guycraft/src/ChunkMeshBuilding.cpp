#include "ChunkMeshBuilding.h"
#include "ChunkManager.h"

ChunkMeshBuilding* ChunkMeshBuilding::m_instance = nullptr;

void useThreadChunkMeshBuilding() {
	auto chManager = ChunkManager::GetInstance();
	auto queueJob = &chManager->chunkMeshBuilding.m_queueJob;
	auto queueComplate = &chManager->chunkMeshBuilding.m_queueComplete;
	while (true) {
		if (chManager->m_queueUnload.size() > 0) continue;
		//we should wait delete chunk all successfully 
		//before run this.
		if (queueJob->empty()) continue;

		auto chunk = queueJob->getFront();
		chunk->mutex.lock();
		if (chunk->isNeedGenerateMesh == false) continue;
		chunk->generateMeshChunk();
		chunk->mutex.unlock();
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
		//check 
		auto chunk = m_queueComplete.front();
		if (chunk->isNeedGenerateMesh or chunk->isGenerateMesh or chunk->isNeedRegenerateMesh) {
			continue;
		}
		m_queueComplete.getFront();//pop queue
		chunk->mesh.transferToGPU();
	}
}

void ChunkMeshBuilding::addQueue(Chunk* chunk)
{
	m_queueJob.push(chunk);
}