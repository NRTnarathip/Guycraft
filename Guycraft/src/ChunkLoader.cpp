#include "ChunkLoader.h"
#include "ChunkManager.h"
#include "ClientEngine.h"
#include "CameraManager.h"

#include "Scenes/SceneMainGame.h"

void useThreadPopulate() {
	auto scMainGame = SceneManager::GetInstance()->getScene<SceneMainGame>(1);

	auto cManager = ChunkManager::GetInstance();
	TerrainGen terrainGen;
	auto cLoader = &cManager->chunkLoader;
	auto queueJob = &cManager->chunkLoader.m_queueJobPopulate;
	while (not scMainGame->isNeedExitToLobby) {
		queueJob->lock();
		if (queueJob->empty()) {
			queueJob->unlock();
			continue;
		}
		auto posJob = queueJob->getFront();
		queueJob->unlock();

		auto job = new JobPopulate();
		job->pos = posJob;
		terrainGen.populate(job);
		cLoader->m_queueJobPopulateComplete.pushLock(job);
	}
}
void ChunkLoader::startThread()
{
	m_threadPopulate = std::move(std::thread(useThreadPopulate));
}

void ChunkLoader::firstLoader(glm::ivec2 pos) {
	startThread();
	lastPosChunk = pos;
	progressLoadChunk(pos);

}
void ChunkLoader::update(glm::ivec2 posPlayer) {
	if (lastPosChunk != posPlayer) {
		lastPosChunk = posPlayer;
		//impl unload chunk
		onPlayerMoveToNewChunk();
		//impl request load chunk
		progressLoadChunk(lastPosChunk);
	}

	//impl packet voxel data and create chunk
	auto manager = ChunkManager::GetInstance();
	while(true) {
		m_queueJobPopulateComplete.lock();
		if (m_queueJobPopulateComplete.empty()) {
			m_queueJobPopulateComplete.unlock();
			break;
		}
		auto job = m_queueJobPopulateComplete.getFront();
		m_queueJobPopulateComplete.unlock();
		auto pos = job->pos;
		if (not m_allocateChunk.has(pos)) {
			delete job;
			continue;
		}

		Chunk* chunk = nullptr;
		manager->chunks.lock();
		//create new chunk;
		if (not manager->chunks.exist(pos)) {
			chunk = manager->chunkPooling.get();
			//setup chunk
			chunk->pos = pos;
			chunk->changeVoxels(job->voxels);
			delete job;

			//add chunk container
			manager->chunks.add(chunk);
			//setup chunk neighbor
			chunk->lock();

			chunk->m_allocateChunkNeighbor = 0;
			Chunk* chunksNeighbor[8];
			glm::ivec2 posChunkNeighbor[8] = {
				glm::ivec2(0, CHUNK_SIZE) + pos,//north
				glm::ivec2(0, -CHUNK_SIZE) + pos,//south
				glm::ivec2(CHUNK_SIZE, 0) + pos,//east
				glm::ivec2(-CHUNK_SIZE, 0) + pos,//west
				pos + glm::ivec2(CHUNK_SIZE, CHUNK_SIZE),//north east
				pos + glm::ivec2(CHUNK_SIZE, -CHUNK_SIZE),//south east
				pos + glm::ivec2(-CHUNK_SIZE, -CHUNK_SIZE),//south west
				pos + glm::ivec2(-CHUNK_SIZE, CHUNK_SIZE),//north west
			};
			for (int i = 0; i < 8; i++) {
				auto posNext = posChunkNeighbor[i];
				auto nextChunk = manager->getChunk(posNext);
				if (nextChunk == nullptr) {
					nextChunk = nullptr;
				}
				chunksNeighbor[i] = nextChunk;
				//set chunk allocate
				bool isExist = m_allocateChunk.has(posNext);
				if (isExist) {
					chunk->m_allocateChunkNeighbor++;
				}
			}
			//link chunk neightbor, genmesh
			chunk->mutexNeighbor.lock();
			chunk->linkChunkNeighbor(chunksNeighbor);
			chunk->mutexNeighbor.unlock();

			//on chunk loaded
			chunk->isLoad = true;
			chunk->isShouldUnload = false;
			chunk->unlock();
		}
		//update chunk voxel data
		else {
			chunk = manager->chunks.m_container[pos];
			chunk->changeVoxels(job->voxels);
			delete job;
		}
		manager->chunks.unlock();
		auto genMesh = ChunkMeshBuilding::GetInstance();
		genMesh->addQueue(chunk,9,true,true);
		//onNewChunk should regenmesh AtNeighbor
		auto cnear = chunk->getAllChunkNeighbor();
		for (auto c : cnear) {
			if (c == nullptr) continue;
			genMesh->addQueue(c, 9, true, false);
		}
	}
}
int ChunkLoader::getAllocateChunkNeighbor(Chunk* chunk)
{
	auto pos = chunk->pos;
	int count = 0;
	for (int i = 0; i < 8; i++) {
		if (m_allocateChunk.has(chunkNeighborPositions[i] + pos)) {
			count++;
		}
	}
	return count;
}
void ChunkLoader::onPlayerMoveToNewChunk()
{
	auto cManager = ChunkManager::GetInstance();
	std::vector<Chunk*> listUnloadChunk;
	//marker chunk is should unload;
	for (auto &elem : cManager->chunks.m_container) {
		auto chunk = elem.second;
		auto posChunk = chunk->pos;
		if (not ChunkManager::ChunkInRange(lastPosChunk, posChunk)) {
			chunk->isShouldUnload = true;
			m_allocateChunk.lock();
			m_allocateChunk.m_map.erase(posChunk);
			m_allocateChunk.unlock();
			listUnloadChunk.push_back(chunk);
		}
	}
	//marker chunk loader edge
	//to do refresh allocate chunk neighbor
	for (auto chunk : listUnloadChunk) {
		chunk->lock();
		unloadChunk(chunk);
		chunk->unlock();
	}
	std::vector<glm::ivec2> listAllocatePosChunk;
	for (auto elem : m_allocateChunk.m_map) {
		auto pos = elem.first;
		if (not cManager->ChunkInRange(lastPosChunk, pos)) {
			listAllocatePosChunk.push_back(pos);
		}
	}
	for (auto pos : listAllocatePosChunk) {
		m_allocateChunk.lock();
		m_allocateChunk.m_map.erase(pos);
		m_allocateChunk.unlock();
	}
	listUnloadChunk.clear();
}

void ChunkLoader::unloadChunk(Chunk* chunk)
{
	chunk->unload();
	auto cm = ChunkManager::GetInstance();
	auto pos = chunk->pos;
	cm->chunks.del(pos);
	cm->chunkPooling.collectPooling(chunk);
}

void ChunkLoader::progressLoadChunk(glm::ivec2 posInit)
{
	SmartQueue<glm::ivec2> queueLoadChunk;
	queueLoadChunk.push(posInit);

	std::unordered_map<glm::ivec2, bool> m_containerPosProgress;
	m_containerPosProgress.emplace(posInit, false);

	auto cManager = ChunkManager::GetInstance();
	SmartQueue<glm::ivec2> m_queueNewRequestChunk;
	//check chunk need to request load
	while (queueLoadChunk.size() > 0) {
		auto pos = queueLoadChunk.getFront();
		if (ChunkManager::ChunkInRange(lastPosChunk, pos)) {
			if (not m_allocateChunk.has(pos)) {
				m_allocateChunk.add(pos, true);
				m_queueNewRequestChunk.push(pos);
			};
			glm::ivec2 posNextChunk[4] = {
				glm::ivec2(0, CHUNK_SIZE) + pos,
				glm::ivec2(0, -CHUNK_SIZE) + pos,
				glm::ivec2(CHUNK_SIZE, 0) + pos,
				glm::ivec2(-CHUNK_SIZE, 0) + pos,
			};
			for (auto posNext : posNextChunk) {
				if (m_containerPosProgress.find(posNext) == m_containerPosProgress.end()) {
					if (ChunkManager::ChunkInRange(lastPosChunk, posNext)) {
						queueLoadChunk.push(posNext);
						m_containerPosProgress.emplace(posNext, true);
					}
				}
			}
		}
	}

	//setup chunk after load;
	while (m_queueNewRequestChunk.size() > 0) {
		auto pos = m_queueNewRequestChunk.getFront();
		if (cManager->chunks.exist(pos)) {
			printf("you request job populate replace\n");
		}
		m_queueJobPopulate.pushLock(pos);
	}
}
