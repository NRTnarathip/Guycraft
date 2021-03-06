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
    auto texture = new Texture(pathFile);
    m_textures.emplace(pathFile, texture);
    return texture;
}
Sprite* ResourceManager::getSprite(std::string spriteID)
{
    return m_sprites[spriteID];
}
void ResourceManager::loadAllResouces() {
    namespace fs = std::filesystem;
    //load resouce all shader
    {
        std::string pathShaders = "assets/shader";
        int isFileExist = 0;
        for (const auto& entry : fs::directory_iterator(pathShaders)) {
            if (isFileExist > 0) {
                isFileExist = 0;
                continue;
            }

            std::string filename = entry.path().string();
            std::string pathFileShader = entry.path().string();

            filename.replace(0, pathShaders.length() + 1, "");
            filename = filename.substr(0, filename.length() - 5);

            std::replace(pathFileShader.begin(), pathFileShader.end(), '\\', '/');
            pathFileShader = pathFileShader.substr(0, pathFileShader.length() - 5);
            addShader(pathFileShader, filename);
            isFileExist++;
        }
    }
    
    //load all texture and mipmap
    {
        std::vector<std::string> listPathTextures = {
        "assets/texture/gui",
        "assets/texture/block",
        };
        for (auto pathTextures : listPathTextures) {
            for (const auto& entry : fs::directory_iterator(pathTextures)) {
                std::string pathFile = entry.path().string();
                std::replace(pathFile.begin(), pathFile.end(), '\\', '/');
                addTexture(pathFile.c_str(), true);
            }
        }
    }
    //setup texture atlas
    //generate texture atlas
    auto texAtlas = new TextureAtlas("chunk", "assets/atlas/test_atlas_0.png");
    m_textureAtlas.emplace(texAtlas->getID(), texAtlas);
    //setup sprite for render 2D GUI
    //manual create sprite
    m_sprites.emplace("assets/texture/gui/button_0",
        new Sprite(m_textures["assets/texture/gui/button_0.png"]));

    m_sprites.emplace("assets/texture/gui/cursor",
        new Sprite(m_textures["assets/texture/gui/cursor.png"]));
}