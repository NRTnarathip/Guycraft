#pragma once
#include "UIComponent.h"
#include <string>
#include "UIObject.h"
class UIText : public UIComponent {
public:
	UIText();
	class Alignment {
	public:
		enum LineAdjustment {
			Left, Center, Right
		};
		LineAdjustment lineVertical = Center;
		LineAdjustment lineHorizontal = Center;
	};

	std::string text = "empty text";
	void render();
	glm::vec3 fontColor = glm::vec3(1.f);
	int fontSize = 24;
	Alignment alignment;
};