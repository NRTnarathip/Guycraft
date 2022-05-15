#pragma once
#include "Texture.h"

class Sprite {
public:
	Sprite(Texture* texture);
	Texture* refTexture = nullptr;
};