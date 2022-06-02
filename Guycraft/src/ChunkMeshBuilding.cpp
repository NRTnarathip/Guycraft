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
	auto cLoader = &cManager->chunkLoader;

	while (not scMainGame->isNeedExitToLobby) {
		queueJob->lock();
		if (queueJob->empty()) {
			queueJob->unlock();
			continue;
		}
		auto job = queueJob->getFront();
		queueJob->unlock();

		auto chunkSection = job.chunkSection;
		if (chunkSection->m_mesh->stateGenerateMesh == 0) {
			continue;
		}

		auto chunk = chunkSection->chunk;
		auto allocateNeighbor = cLoader->getAllocateChunkNeighbor(chunk);
		if (chunk->getHasChunkNeighborCount() != allocateNeighbor) {
			queueJob->pushFrontLock({chunkSection});
			continue;
		}
		chunkSection->generateMesh();
	}
}
void ChunkMeshBuilding::startWithThread()
{
	int numThreadMax = 1;
	for (int i = 0; i < numThreadMax; i++) {
		m_threads.push_back(std::move(std::thread(useThreadChunkMeshBuilding)));
	}
}

void ChunkMeshBuilding::updateMainThread()
{
	while(true) {
		m_queueComplete.lock();
		if (m_queueComplete.empty()) {
			m_queueComplete.unlock();
			return;
		}
		auto mesh = m_queueComplete.getFront();
		m_queueComplete.unlock();

		if (mesh->m_chunk->chunk->isLoad == false) {
			continue;
		}
		mesh->fluid.makeCopyDataJobToComplete();
		mesh->fluid.transferToGPU();
		mesh->solid.makeCopyDataJobToComplete();
		mesh->solid.transferToGPU();
		auto chunk = mesh->m_chunk->chunk;
		chunk->m_meshsActive.addNoneExist(mesh->m_index, mesh);
	}
}

void ChunkMeshBuilding::addQueue(Chunk* chunk, int index,
	bool isPushFront,bool isDontPushIfExist) {

	if (index == CHUNK_SIZE) {
		for (u8 i = 0; i < CHUNK_SIZE; i++) {
			int chunkSectionIndex = CHUNK_SIZE_INDEX - i;
			auto cs = chunk->m_chunks[chunkSectionIndex];
			if (cs->m_isBlocksEmpty) continue;

			auto mesh = cs->m_mesh;
			mesh->lock();
			int state = mesh->stateGenerateMesh;
			if (isDontPushIfExist and state >= 1) {
				mesh->unlock();
				continue;
			}
			mesh->stateGenerateMesh = StateGenerateMesh::OnNeedGenerate;
			mesh->unlock();
			if (isPushFront)
				m_queueJob.pushFrontLock({ cs });
			else
				m_queueJob.pushLock({ cs });
		}
		return;
	}
	auto cs = chunk->m_chunks[index];
	if (cs->m_isBlocksEmpty) return;

	auto mesh = cs->m_mesh;

	mesh->lock();
	if (isDontPushIfExist and 
		mesh->stateGenerateMesh >= StateGenerateMesh::OnNeedGenerate) {
		mesh->unlock();
		return;
	}
	auto pos = mesh->m_chunk->pos;
	mesh->stateGenerateMesh = StateGenerateMesh::OnNeedGenerate;
	mesh->unlock();

	if (isPushFront) {
		m_queueJob.pushFrontLock({ chunk->m_chunks[index] });
		return;
	}
	m_queueJob.pushLock({ chunk->m_chunks[index]});
}

void ChunkMeshBuilding::insertQueueAtFront(std::vector<JobGenerateMesh> jobs, bool isDontPushIfExist)
{
	std::vector<JobGenerateMesh> mainJobs;

	for(auto job : jobs) {
		auto chunkSection = job.chunkSection;
		auto mesh = chunkSection->m_mesh;
		mesh->lock();
		if (isDontPushIfExist and
			mesh->stateGenerateMesh >= StateGenerateMesh::OnNeedGenerate) {
			mesh->unlock();

			continue;
		}
		mesh->stateGenerateMesh = StateGenerateMesh::OnNeedGenerate;
		mesh->unlock();
		mainJobs.push_back({ chunkSection });
	}
	m_queueJob.insertFrontLock(mainJobs);
}
