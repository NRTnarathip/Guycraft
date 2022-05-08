#include <GenerateMeshChunk.h>
#include <PlayerController.h>
#include <ClientEngine.h>
#include <SmartChunk.h>
#include <ChunkManager.h>

void useThreadGenMesh(GenMeshChunk* ref) {
	auto chManager = ChunkManager::GetInstance();
	auto queNeedGen = &chManager->queNeedGenMeshChunkGroup;
	while (true) {

		if (chManager->queDeleteChunk.size() > 0) continue;
		//we should wait delete chunk all successfully 
		//before run this.

		auto smChunk = queNeedGen->getFront();
		if (smChunk == NULL) continue;

		smChunk->lock();
		if (smChunk->chunkIsEmty()) {
			smChunk->unlock();
			continue;
		} 

		auto cg = smChunk->get();
		if (cg->needGenMeshChunk == false) {
			smChunk->unlock();
			continue;
		}

		//generate mesh chunk
		for (auto c : cg->chunks) {
			c->generateMeshChunk();
		}
		cg->needGenMeshChunk = false;

		smChunk->unlock();
		//shared thread
		ref->queTransferGPU.push(smChunk);
	}
}
void GenMeshChunk::init() {
	std::thread thGenMesh(useThreadGenMesh, this);
	listThreadGenMesh.push_back(std::move(thGenMesh));
	//run only main threading
	//we should transfer data chunk mesh to GPU
}
void GenMeshChunk::update() { //update every frame
	int maxQuePerUpdate = 5;
	for(int i =0;i< maxQuePerUpdate;i++) {
		auto smChunk = queTransferGPU.getFront();
		if (smChunk == NULL) break;

		smChunk->lock();
		if (smChunk->chunkIsEmty()) {
			smChunk->unlock();
			continue;
		}

		for (auto c : smChunk->get()->chunks) {
			c->mesh.transferToGPU();
		}

		smChunk->unlock();
	}
}
