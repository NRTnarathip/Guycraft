#pragma once
#include "Physics/Transform.h"
#include "Sprite.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "RectTransform.h"

class SpriteRenderer {
public:
	SpriteRenderer() {}
	SpriteRenderer(Sprite* ref) {
		sprite = ref;
	}
	Sprite *sprite = nullptr;
	glm::vec4 color = glm::vec4(1.f);
	void draw();
};