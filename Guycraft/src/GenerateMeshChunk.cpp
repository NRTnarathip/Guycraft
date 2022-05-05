#include <GenerateMeshChunk.h>
#include <PlayerController.h>
#include <ClientEngine.h>
#include <SmartChunk.h>
void useThreadGenMesh(GenMeshChunk* ref) {
	auto queNeedGen = &ref->chManager->queNeedGenMeshChunkGroup;
	while (true) {

		if (ref->chManager->queDeleteChunk.size() > 0) continue;
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
void GenMeshChunk::init(ChunkManager* chunkManager) {
	chManager = chunkManager;

	std::thread thGenMesh(useThreadGenMesh, this);
	//thGenMesh.detach();

	listThreadGenMesh.push_back( std::move(thGenMesh));
	
	//run only main threading
	//we should transfer data chunk mesh to GPU
	return;
}