#include "ChunkMeshBuilding.h"
#include "ChunkManager.h"
#include <chrono>
#include "SceneManager.h"
#include "Scenes/SceneMainGame.h"

ChunkMeshBuilding* ChunkMeshBuilding::m_instance = nullptr;

void useThreadChunkMeshBuilding() {
	auto chManager = ChunkManager::GetInstance();
	auto cMeshBuilder = &chManager->chunkMeshBuilding;
	auto queueJob = &chManager->chunkMeshBuilding.m_queueJob;
	auto queueComplate = &chManager->chunkMeshBuilding.m_queueComplete;
	auto scMainGame = SceneManager::GetInstance()->getScene<SceneMainGame>(1);
	while (not scMainGame->isNeedExitToLobby) {
		queueJob->lock();
		if (queueJob->empty()) {
			queueJob->unlock();
			continue;
		} 
		auto chunk = queueJob->getFront();
		queueJob->unlock();
		chunk->lock();
		if (not chunk->isNeedGenerateMesh or not chunk->isLoad) {
			chunk->unlock();
			continue;
		}
		int chunkHasNeighborCount = chunk->getChunkNieghborCount();
		int allocChunkNeighborCount = chunk->m_allocateChunkNeighborCount;
		if (chunkHasNeighborCount != allocChunkNeighborCount) {
			int newAlloc = ChunkManager::GetInstance()->chunkLoader.getAllocateChunkNeighborCount(chunk);
			if (chunkHasNeighborCount != newAlloc) {
				queueJob->pushLock(chunk);
				chunk->unlock();
				continue;
			}
			chunk->m_allocateChunkNeighborCount = newAlloc;
		}
		auto start = std::chrono::high_resolution_clock::now();
		chunk->generateMeshChunk();
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		/*printf("gen mesh block time:%d ms\n",
			std::chrono::nanoseconds(elapsed).count() / 100000);*/
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

		if (mesh->isOnGenerate) {
			continue;
		}

		mesh->transferToGPU();
		mesh->isComplete = true;
	}
}

void ChunkMeshBuilding::addQueue(Chunk* chunk)
{
	chunk->isNeedGenerateMesh = true;
	m_queueJob.pushLock(chunk);
}

void ChunkMeshBuilding::addQueueFront(Chunk* chunk)
{
	chunk->isNeedGenerateMesh = true;
	m_queueJob.lock();
	m_queueJob.pushFront(chunk);
	m_queueJob.unlock();
}