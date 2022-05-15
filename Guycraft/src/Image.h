#pragma once
#include "UIObject.h"
#include "SpriteRenderer.h"
#include "RectTransform.h"
#include <glm/vec4.hpp>
#include "UIComponent.h"
class GUI;

class Image : public UIComponent, public SpriteRenderer {
public:
	void render();
	void init();
};