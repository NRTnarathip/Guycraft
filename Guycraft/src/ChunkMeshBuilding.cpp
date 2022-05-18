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
		if (chunk->getChunkNieghborCount() <= 2) {
			chunk->unlock();
			queueJob->pushLock(chunk);
			continue;
		}
		auto start = std::chrono::high_resolution_clock::now();
		chunk->generateMeshChunk();
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		printf("gen mesh block time:%d ms\n",
			std::chrono::nanoseconds(elapsed).count() / 100000);
		//regenerate mesh when chunk neighbor not complete
		if (chunk->getChunkNieghborCount() != chunk->m_allocateChunkNeighborCount) {
			cMeshBuilder->addQueue(chunk);
		}
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
	m_queueJob.pushLock(chunk);
}

void ChunkMeshBuilding::genMeshChunkNeighborEdge(Chunk* c)
{
	for (auto chunk : { c->north, c->south,c->east, c->west }) {
		if (chunk == nullptr) continue;
		chunk->lock();
		chunk->isNeedGenerateMesh = true;
		chunk->generateMeshChunk();
		chunk->unlock();
	}
}
