#include "BlockSystem/BlockDatabase.h"
#include "ResourceManager.h"

BlockDatabase* BlockDatabase::m_instance = nullptr;

void BlockDatabase::init()
{
	using FaceDir = BlockModel::FaceDirection;

	//setup texture uv atlas
	auto atlas = ResourceManager::GetInstance()->m_textureAtlas["chunk"];
	atlas->addTextureUV("dirt", { 1, 0 });
	atlas->addTextureUV("dirt_side", { 2, 0 });
	atlas->addTextureUV("dirt_grass", { 3, 0 });
	atlas->addTextureUV("stone", { 4, 0 });
	atlas->addTextureUV("sand", { 5, 0 });
	atlas->addTextureUV("water", { 6, 0 });


	auto *dirt = new BlockModel(1, Shape::Cube);
	dirt->addTexture(FaceDir::Side, "dirt_side");
	dirt->addTexture(FaceDir::Top, "dirt_grass");
	dirt->addTexture(FaceDir::Buttom, "dirt");

	auto* stone = new BlockModel(2, Shape::Cube);
	stone->addTexture(FaceDir::All, "stone");
	auto* sand = new BlockModel(3, Shape::Cube);
	sand->addTexture(FaceDir::All, "sand");

	auto* water = new BlockModel(4, Shape::Water);
	water->addTexture(FaceDir::All, "water");

	addModels({ new BlockModel(0, Shape::Invisible),
		dirt, stone, sand, water});
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
