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
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	while (not scMainGame->isNeedExitToLobby) {
		queueJob->lock();
		if (queueJob->empty()) {
			queueJob->unlock();
			continue;
		} 
		auto job = queueJob->getFront();
		queueJob->unlock();
		auto chunk = job.chunk;
		auto mesh = &chunk->meshs[job.voxelGroup];
		mesh->lock();
		if (mesh->isNeedGenMesh == false) {
			mesh->unlock();
			continue;
		}
		mesh->unlock();

		chunk->lock();
		if (chunk->getHasChunkNeighborCount() != chunk->m_allocateChunkNeighbor) {
			chunk->m_allocateChunkNeighbor = cManager->chunkLoader.getAllocateChunkNeighbor(chunk);
			chunk->unlock();
			queueJob->pushLock(job);
			continue;
		}
	

		/* Getting number of milliseconds as an integer. */
		auto t1 = high_resolution_clock::now();
		chunk->generateMesh(job.voxelGroup);
		auto t2 = high_resolution_clock::now();
		auto ms_int = duration_cast<milliseconds>(t2 - t1);
		std::cout << "generate mesh chunk: " << ms_int.count() << "ms\n";

		chunk->unlock();
	}
}
void ChunkMeshBuilding::startWithThread()
{
	m_thread = std::move(std::thread(useThreadChunkMeshBuilding));
}

void ChunkMeshBuilding::updateMainThread()
{
	while (true) {
		m_queueComplete.lock();
		if (m_queueComplete.empty()) {
			m_queueComplete.unlock();
			return;
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
		mesh->isActive = true;
		mesh->chunk->m_meshsActive.m_map[mesh->pos.y / CHUNK_SIZE] = mesh;
		mesh->unlock();
	}
}

void ChunkMeshBuilding::addQueue(Chunk* chunk, int voxelGroup, 
	bool isPushFront,bool isGenForChunkNeighbor) {

	if (voxelGroup == VOXELGROUP_COUNT) {
		for (u8 i = 0; i < VOXELGROUP_COUNT; i++) {
			if (chunk->voxelGroupEmpty[i]) { return; }
			auto mesh = &chunk->meshs[i];
			mesh->lock();
			if (isGenForChunkNeighbor and mesh->isNeedGenMesh) {
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
	if (chunk->voxelGroupEmpty[voxelGroup]) { return; }
	auto mesh = &chunk->meshs[voxelGroup];
	mesh->lock();
	if (isGenForChunkNeighbor and mesh->isNeedGenMesh) {
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