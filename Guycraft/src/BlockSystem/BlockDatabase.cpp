#include "BlockSystem/BlockDatabase.h"

BlockDatabase* BlockDatabase::m_instance = nullptr;

void BlockDatabase::init()
{
	using FaceDir = BlockModel::FaceDirection;

	m_texturesTileIndex.emplace("dirt", 1);
	m_texturesTileIndex.emplace("stone", 2);
	m_texturesTileIndex.emplace("sand", 3);
	m_texturesTileIndex.emplace("water", 4);
	auto* air = new BlockModel(0, -1);

	auto *dirt = new BlockModel(1, 0);
	dirt->addTexture(FaceDir::All, "dirt");
	auto* stone = new BlockModel(2, 0);
	stone->addTexture(FaceDir::All, "stone");
	auto* sand = new BlockModel(3, 0);
	sand->addTexture(FaceDir::All, "sand");

	auto* water = new BlockModel(4, BlockModel::Shape::Fluid);
	water->addTexture(FaceDir::All, "water");

	addModels({air, dirt, stone,sand,water});
}

void BlockDatabase::addModel(BlockModel* model)
{
	m_models.emplace(model->m_type, model);
}

void BlockDatabase::addModels(std::vector<BlockModel*> models)
{
	for (auto model : models) {
		addModel(model);
	}
}
