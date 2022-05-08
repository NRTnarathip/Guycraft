#include "ResourceManager.h"
#include "ClientEngine.h"
#include <string>
#include <iostream>
#include <filesystem>
#include <algorithm>

ResourceManager* ResourceManager::instance = nullptr;

Shader* ResourceManager::addShader(std::string pathVertexAndFragment, std::string shaderName) {
    auto pathVert = pathVertexAndFragment + ".vert";
    auto pathFrag = pathVertexAndFragment + ".frag";
    auto newShader = new Shader(pathVert.c_str(), pathFrag.c_str());
    m_shaders.emplace(shaderName, newShader);
    return newShader;
}

Texture* ResourceManager::addTexture(const char* pathFile, bool isMipmapping) {
    auto texture = new Texture(pathFile, isMipmapping, GL_CLAMP_TO_BORDER, GL_NEAREST);
    m_textures.emplace(pathFile, texture);
    return texture;
}
Sprite* ResourceManager::getSprite(std::string spriteID)
{
    return m_sprites[ "assets/textures/" + spriteID];
}
void ResourceManager::loadAllResouces() {
    namespace fs = std::filesystem;
    //load resouce all shader
    std::string pathShaders = "src/Shaders";
    int isFileExist = 0;
    for (const auto& entry : fs::directory_iterator(pathShaders)) {
        if (isFileExist > 0) {
            isFileExist = 0;
            continue;
        }

        std::string filename = entry.path().string();
        std::string pathFileShader = entry.path().string();

        filename.replace(0, pathShaders.length()+1, "");
        filename = filename.substr(0, filename.length() - 5);

        std::replace(pathFileShader.begin(), pathFileShader.end(), '\\', '/');
        pathFileShader = pathFileShader.substr(0, pathFileShader.length() - 5);
        addShader(pathFileShader, filename);
        isFileExist++;
    }
    std::vector<std::string> listPathTextures = {
    "assets/textures/gui",
    "assets/textures/blocks", };
    for (auto pathTextures : listPathTextures) {
        for (const auto& entry : fs::directory_iterator(pathTextures)) {
            std::string spriteName = entry.path().string();

            spriteName.replace(0, pathTextures.length() + 1, "");
            spriteName = spriteName.substr(0, spriteName.length() - 4);

            std::string pathFile = entry.path().string();
            std::replace(pathFile.begin(), pathFile.end(), '\\', '/');
            auto texture = addTexture(pathFile.c_str(), false);

            auto spriteKeyName = pathTextures + "/" + spriteName;

            auto newSprite = new Sprite(texture);
            newSprite->setupRenderData();
            m_sprites.emplace(spriteKeyName, newSprite);
        }
    }
}