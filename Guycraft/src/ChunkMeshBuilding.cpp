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
		if (chunk->isNeedGenerateMesh == false or chunk->isFourceStopGenerateMesh) {
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
	m_thread = std::move(std::thread(useThreadChunkMeshBuilding));
}

void ChunkMeshBuilding::updateMainThread()
{
	while(true) {
		m_queueComplete.lock();
		if (m_queueComplete.empty()) {
			m_queueComplete.unlock();
			return;
		}
		auto chunk = m_queueComplete.getFront();
		m_queueComplete.unlock();

		chunk->lock();
		chunk->mesh.transferToGPU();
		chunk->unlock();
	}
}

void ChunkMeshBuilding::addQueue(Chunk* chunk)
{
	m_queueJob.lock();
	chunk->isNeedGenerateMesh = true;
	chunk->isFourceStopGenerateMesh = false;
	m_queueJob.push(chunk);
	m_queueJob.unlock();
}