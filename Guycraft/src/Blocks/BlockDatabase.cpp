#include "BlockSystem/BlockDatabase.h"

BlockDatabase* BlockDatabase::m_instance = nullptr;

void BlockDatabase::init()
{
	using FaceDir = BlockModel::FaceDirection;

	m_texturesTileIndex.emplace("dirt", 1);
	m_texturesTileIndex.emplace("stone", 2);
	m_texturesTileIndex.emplace("sand", 3);

	auto *dirt = new BlockModel(1);
	dirt->addTexture(FaceDir::All, "dirt");
	auto* stone = new BlockModel(2);
	stone->addTexture(FaceDir::All, "stone");
	auto* sand = new BlockModel(3);
	sand->addTexture(FaceDir::All, "sand");

	m_models.emplace(dirt->voxelType, dirt);
	m_models.emplace(stone->voxelType, stone);
	m_models.emplace(sand->voxelType, sand);
}
