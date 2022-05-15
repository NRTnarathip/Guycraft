#pragma once
#include "Physics/Transform.h"
#include "Sprite.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "RectTransform.h"
#include "OpenGL Class/VAO.h"
#include "OpenGL Class/VBOAndEBO.h"
class SpriteRenderer {
public:
	VAO vao;
	VBO vbo;
	Sprite* m_sprite = nullptr;
	glm::vec4 color = glm::vec4(1.f);
	void initRenderer();
	void setSprite(Sprite* sprite);
	void draw();
};