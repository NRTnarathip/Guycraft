#pragma once
#include "Texture.h"
#include "OpenGL Class/VAO.h"
#include "OpenGL Class/VBOAndEBO.h"

class Sprite {
public:
	Sprite(Texture* texture);
	VAO vao;
	VBO vbo;
	Texture* refTexture = nullptr;
	void setupRenderData();
};