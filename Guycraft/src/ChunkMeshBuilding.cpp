#include "ChunkMeshBuilding.h"
#include "ChunkManager.h"

ChunkMeshBuilding* ChunkMeshBuilding::m_instance = nullptr;

void useThreadChunkMeshBuilding() {
	auto chManager = ChunkManager::GetInstance();
	auto queueJob = &chManager->chunkMeshBuilding.m_queueJob;
	auto queueComplate = &chManager->chunkMeshBuilding.m_queueComplete;
	while (true) {
		queueJob->lock();
		if (queueJob->empty()) {
			queueJob->unlock();
			continue;
		} 
		auto chunk = queueJob->getFront();
		queueJob->unlock();

		chunk->lock();
		if (chunk->isNeedGenerateMesh == false
			or chunk->isFourceUnload) {
			chunk->unlock();
			continue;
		}

		chunk->generateMeshChunk();
		chunk->unlock();

		queueComplate->lock();
		queueComplate->push(chunk);
		queueComplate->unlock();
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
		m_queueComplete.lock();
		if (m_queueComplete.empty()) {
			m_queueComplete.unlock();
			continue;
		}
		auto chunk = m_queueComplete.getFront();
		m_queueComplete.unlock();

		chunk->lock();
		if (chunk->isNeedGenerateMesh or chunk->isGenerateMesh or chunk->isNeedRegenerateMesh) {
			chunk->unlock();
			continue;
		}

		chunk->mesh.transferToGPU();
		chunk->unlock();
	}
}

void ChunkMeshBuilding::addQueue(Chunk* chunk)
{
	m_queueJob.lock();
	m_queueJob.push(chunk);
	m_queueJob.unlock();
}