#include <ChunkManager.h>
#include <Game.h>
#include "glm/gtx/string_cast.hpp"
#include <array>
#include <SmartQueue.h>
#include "CameraManager.h"
#include "ClientEngine.h"

ChunkManager* ChunkManager::m_instance = nullptr;
bool inRange(int value, int min, int max)
{
	return (value >= min) and (value <= max);
}
void ChunkManager::addQueueDestroyBlock(glm::ivec3 blockWorldPos)
{
	m_queueDestroyBlock.pushLock(blockWorldPos);
}
void ChunkManager::init() {
	lastViewPos = CameraManager::GetCurrentCamera()->transform.position;
	chunkMeshBuilding.startWithThread();
	auto camera = CameraManager::GetCurrentCamera();
	auto posPlayerToChunk = ToChunkPosition(camera->transform.position);
	chunkLoader.firstLoader(posPlayerToChunk);

	auto res = ResourceManager::GetInstance();
	auto sdSolid = res->m_shaders["chunk_block_solid"];
	auto sdFluid = res->m_shaders["chunk_block_fluid"];
	auto atlasChunk = res->m_textureAtlas["chunk"];
	for (auto s : { sdSolid, sdFluid }) {
		s->Bind();
		s->SetVar("tex", 0);
		s->SetFloat("aoStrength", 0.45f);
		s->SetVec2("textureAtlasSize", atlasChunk->getSize());
	}
}
glm::vec3 ChunkManager::ToChunkPosition(glm::vec3 worldPos) const
{
	worldPos.y = glm::clamp(worldPos.y, 0.f, (float)CHUNK_HEIGHT_INDEX);
	int x = floor(worldPos.x / CHUNK_SIZE);
	int y = floor(worldPos.y / CHUNK_SIZE);
	int z = floor(worldPos.z / CHUNK_SIZE);

	return { x << 4, y << 4, z << 4 };
}
void ChunkManager::update() {
	auto camera = CameraManager::GetCurrentCamera();
	auto posCamera = camera->transform.position;
	auto posPlayerToChunk = ToChunkPosition(posCamera);

	//check if chunk is not use or out of range render system
	chunkLoader.update({ posPlayerToChunk.x, posPlayerToChunk.z });

	//update destroy block
	Block voxReplace;
	voxReplace.data = 0;
	voxReplace.type = 0;
	SmartQueue<JobGenerateMesh> queJobGenMesh;
	SmartUnorderMap<glm::ivec3, bool> chunkUpdateVoxel;
	while (m_queueDestroyBlock.size() > 0) {
		glm::ivec3 blockWorldPos = m_queueDestroyBlock.getFront();
		m_queueUpdateBlock.push({ voxReplace, blockWorldPos });
	}

	while (m_queueUpdateBlock.size() > 0) {
		auto job = m_queueUpdateBlock.getFront();

		glm::ivec3 chunkpos = ToChunkPosition(job.worldPos);
		auto chunk = getChunk({ chunkpos.x,chunkpos.z });
		if (chunk == nullptr) continue;

		int csIndex = chunkpos.y / CHUNK_SIZE;
		auto blockPos = job.worldPos - chunkpos;
		auto chunkSection = chunk->m_chunks[csIndex];
		chunkSection->setBlock((blockPos.z << 8) + (blockPos.y << 4) + blockPos.x, job.block);

		if (not chunkUpdateVoxel.has(chunkpos)) {
			chunkUpdateVoxel.add(chunkpos, 1);
			queJobGenMesh.push({ chunkSection });
		}
	}
	std::vector<JobGenerateMesh> jobGenerateMesh;
	while(queJobGenMesh.size() > 0) {
		auto job = queJobGenMesh.getFront();
		auto chunk = job.chunkSection->chunk;
		auto chunksGen = chunk->getAllChunkNeighbor();
		chunksGen.push_back(chunk);

		int chunkIndex = job.chunkSection->m_index;
		for (auto chunk : chunksGen) {
			if(chunk == nullptr) continue;

			jobGenerateMesh.push_back({ chunk->m_chunks[chunkIndex] });
			if (chunkIndex > 0)
				jobGenerateMesh.push_back({ chunk->m_chunks[chunkIndex - 1] });
			if (chunkIndex < CHUNK_SIZE_INDEX)
				jobGenerateMesh.push_back({ chunk->m_chunks[chunkIndex + 1] });
		}
	}
	chunkMeshBuilding.insertQueueAtFront(jobGenerateMesh, false);
}
bool ChunkManager::ChunkInRange(glm::ivec2 playerPos, glm::ivec2 chunkPos)
{
	int rangeRender = ClientEngine::GetInstance().graphicSetting.renderDistance * CHUNK_SIZE;
	int offsX = chunkPos.x - playerPos.x;
	int offsZ = chunkPos.y - playerPos.y;
	if (inRange(offsX, -rangeRender, rangeRender)
		and inRange(offsZ, -rangeRender, rangeRender)) {
		return true;
	}
	return false;
}
void ChunkManager::render() {
	auto res = ResourceManager::GetInstance();

	auto sdSolid = res->m_shaders["chunk_block_solid"];
	auto sdFluid = res->m_shaders["chunk_block_fluid"];
	auto texAtlas = res->m_textureAtlas["chunk"];
	for (auto shader : { sdSolid, sdFluid }) {
		shader->Bind();
		shader->SetFloat("time", Time::lastTime);
		CameraManager::GetInstance().uploadCameraMatrixToShader(shader);
	}
	Shader* shaders[2]{sdSolid, sdFluid};
	for (auto kvp : chunks.m_container) {
		kvp.second->render(shaders);
	}
}
Chunk* ChunkManager::getChunk(glm::ivec2 pos) {
	if (chunks.exist(pos)) {
		return chunks.m_container.at(pos);
	}
	return nullptr;
};