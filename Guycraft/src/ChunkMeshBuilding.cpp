#include "ChunkMeshBuilding.h"
#include "ChunkManager.h"
#include <chrono>

ChunkMeshBuilding* ChunkMeshBuilding::m_instance = nullptr;

void useThreadChunkMeshBuilding() {
	auto chManager = ChunkManager::GetInstance();
	auto cMeshBuilder = &chManager->chunkMeshBuilding;
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
		if (not chunk->isNeedGenerateMesh) {
			chunk->unlock();
			continue;
		}
		auto start = std::chrono::high_resolution_clock::now();
		chunk->generateMeshChunk();
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		/*if (not chunk->isEmpty()) {
			printf("gen mesh block %i time:%d ms\n", chunk->getBlockCount(),
				std::chrono::nanoseconds(elapsed).count() / 100000);
		}*/
		cMeshBuilder->genMeshChunkNeighborEdge(chunk);
		chunk->unlock();
	}
}
void ChunkMeshBuilding::startWithThread()
{
	m_thread = std::move(std::thread(useThreadChunkMeshBuilding));
}

void ChunkMeshBuilding::updateMainThread()
{
	for(int i =0;i < VOXEL_GROUP_COUNT;i++){
		m_queueComplete.lock();
		if (m_queueComplete.empty()) {
			m_queueComplete.unlock();
			return;
		}
		auto mesh = m_queueComplete.getFront();
		m_queueComplete.unlock();
		mesh->lock();
		mesh->transferToGPU();
		mesh->isComplete = true;
		mesh->unlock();
	}
}

void ChunkMeshBuilding::addQueue(Chunk* chunk)
{
	m_queueJob.lock();
	chunk->isNeedGenerateMesh = true;
	m_queueJob.push(chunk);
	m_queueJob.unlock();
}

void ChunkMeshBuilding::genMeshChunkNeighborEdge(Chunk* c)
{
	if (c->north != nullptr) {
		c->north->isNeedGenerateMesh = true;
		c->north->generateMeshChunk();
	}
	if (c->south != nullptr) {
		c->south->isNeedGenerateMesh = true;
		c->south->generateMeshChunk();
	}
	if (c->east != nullptr) {
		c->east->isNeedGenerateMesh = true;
		c->east->generateMeshChunk();
	}
	if (c->west != nullptr) {
		c->west->isNeedGenerateMesh = true;
		c->west->generateMeshChunk();
	}
}
