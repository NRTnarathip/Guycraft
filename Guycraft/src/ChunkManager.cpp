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
	auto sdFulid = res->m_shaders["chunk_block_fluid"];
	sdSolid->Bind();
	sdSolid->SetVar("tex", 0);
	sdSolid->SetFloat("aoStrength", 0.45f);

	sdFulid->Bind();
	sdFulid->SetVar("tex", 0);
	sdFulid->SetFloat("aoStrength", 0.45f);
	sdFulid->SetFloat("time", (float)Time::lastTime);
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
	Voxel voxReplace;
	voxReplace.data = 0;
	voxReplace.type = 0;
	SmartQueue<JobGenerateMesh> queJobGenMesh;
	SmartUnorderMap<glm::ivec3,bool> chunkUpdateVoxel;
	while (m_queueDestroyBlock.size() > 0) {
		glm::ivec3 blockWorldPos = m_queueDestroyBlock.getFront();
		m_queueAddBlock.push({ voxReplace, blockWorldPos });
	}

	while (m_queueAddBlock.size() > 0) {
		auto job = m_queueAddBlock.getFront();

		glm::ivec3 chunkpos = ToChunkPosition(job.worldPos);
		auto chunk = getChunk({ chunkpos.x,chunkpos.z });
		if (chunk == nullptr) continue;

		int voxelGroup = chunkpos.y / CHUNK_SIZE;
		auto blockPos = job.worldPos - chunkpos;
		chunk->voxels[blockPos.x + (blockPos.y << 4) + (blockPos.z << 8) + (voxelGroup << 12)] = job.voxel;
		if (not chunkUpdateVoxel.has(chunkpos)) {
			chunkUpdateVoxel.add(chunkpos, 1);
			queJobGenMesh.push({ chunk, voxelGroup });
		}
	}
	while(queJobGenMesh.empty() == 0) {
		auto job = queJobGenMesh.getFront();
		auto chunk = job.chunk;
		chunkMeshBuilding.addQueue(chunk,job.voxelGroup,true,true);
		//regen chunk neighbor block
		auto cnears = chunk->getAllChunkNeighbor();
		for (auto c : cnears) {
			if (c == nullptr) continue;
			chunkMeshBuilding.addQueue(c, job.voxelGroup, true, true);
		}
	}
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
	auto sdFulid = res->m_shaders["chunk_block_fluid"];
	auto mcatlas = res->m_textures["assets/textures/blocks/mcatlas.png"];
	sdFulid->Bind();
	sdSolid->Bind();
	mcatlas->Activate(GL_TEXTURE0);
	sdFulid->SetFloat("time", Time::lastTime);
	sdSolid->SetFloat("time", Time::lastTime);

	CameraManager::GetInstance().uploadCameraMatrixToShader(sdSolid);
	CameraManager::GetInstance().uploadCameraMatrixToShader(sdFulid);

	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;
	Shader* shaders[2]{sdSolid,sdFulid};
	auto t1 = high_resolution_clock::now();
	for (auto kvp : chunks.m_container) {
		kvp.second->render(shaders);
	}
	//auto t2 = high_resolution_clock::now();
	///* Getting number of milliseconds as an integer. */
	//auto ms_int = duration_cast<milliseconds>(t2 - t1);
	//std::cout << ms_int.count() << " ms of render chunk count " << chunks.m_container.size() << " \n";

}
Chunk* ChunkManager::getChunk(glm::ivec2 pos) {
	if (chunks.exist(pos)) {
		return chunks.m_container.at(pos);
	}
	return nullptr;
};