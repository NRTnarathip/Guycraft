#include "ChunkMeshBuilding.h"
#include "ChunkManager.h"
#include <chrono>
#include "SceneManager.h"
#include "Scenes/SceneMainGame.h"

ChunkMeshBuilding* ChunkMeshBuilding::m_instance = nullptr;

void useThreadChunkMeshBuilding() {
	auto cManager = ChunkManager::GetInstance();
	auto cMeshBuilder = &cManager->chunkMeshBuilding;
	auto queueJob = &cManager->chunkMeshBuilding.m_queueJob;
	auto scMainGame = SceneManager::GetInstance()->getScene<SceneMainGame>(1);
	while (not scMainGame->isNeedExitToLobby) {
		queueJob->lock();
		if (queueJob->empty()) {
			queueJob->unlock();
			continue;
		} 
		auto job = queueJob->getFront();
		queueJob->unlock();
		auto chunk = job.chunk;
		chunk->lock();
		cManager->chunkLoader.m_allocateChunk.lock();
		int newAllocateChunkNeighbor = cManager->chunkLoader.getAllocateChunkNeighbor(chunk);
		cManager->chunkLoader.m_allocateChunk.unlock();
		chunk->m_allocateChunkNeighbor = newAllocateChunkNeighbor;
		if (chunk->getHasChunkNeighborCount() != chunk->m_allocateChunkNeighbor) {
			chunk->unlock();
			queueJob->pushLock(job);
			continue;
		}
		auto mesh = &chunk->meshs[job.voxelGroup];
		mesh->lock();
		if (mesh->isNeedGenMesh == false) {
			mesh->unlock();
			chunk->unlock();
			continue;
		}
		mesh->unlock();
		chunk->generateMesh(job.voxelGroup);
		chunk->unlock();
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
			break;
		}
		auto mesh = m_queueComplete.getFront();
		m_queueComplete.unlock();
		mesh->lock();
		if (mesh->isNeedGenMesh) {
			mesh->unlock();
			continue;
		}
		mesh->fluid.transferToGPU();
		mesh->solid.transferToGPU();
		mesh->isComplete = true;
		mesh->unlock();
	}
}

void ChunkMeshBuilding::addQueue(Chunk* chunk, int voxelGroup, 
	bool isRebuildMesh, bool isPushFront) {

	if (voxelGroup == 9) {
		for (u8 i = 0; i < 8; i++) {
			auto mesh = &chunk->meshs[i];
			mesh->lock();
			if (not isRebuildMesh and mesh->isNeedGenMesh) {
				mesh->unlock();
				continue;
			}
			mesh->isNeedGenMesh = true;
			mesh->unlock();

			if (isPushFront)
				m_queueJob.pushFrontLock({ chunk, i });
			else
				m_queueJob.pushLock({ chunk, i });
		}
		return;
	}
	auto mesh = &chunk->meshs[voxelGroup];
	mesh->lock();
	if (not isRebuildMesh and mesh->isNeedGenMesh) {
		mesh->unlock();
		return;
	}
	mesh->isNeedGenMesh = true;
	mesh->unlock();

	if (isPushFront) {
		m_queueJob.pushFrontLock({ chunk, voxelGroup });
		return;
	}
	m_queueJob.pushLock({chunk, voxelGroup});
}