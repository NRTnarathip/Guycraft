#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

//#include "UIObject.h"
class UIObject;

class RectTransform {
private:
	glm::vec2 inceptGetPosition();
public:
	UIObject* m_uiObject = nullptr;
	RectTransform* parent = nullptr;
	std::vector<RectTransform*> childs;
	glm::vec2 getoffs() {
		return pivot * getsize();
	}

	//have a preset left-top, lfet-down, center, right-top, right-down
	glm::vec2 anchorPreset = { 0.5f, 0.5f };
	glm::vec2 position{ 0, 0 };
	glm::vec2 pivot{ 0.5f, 0.5f };
	glm::vec2 size{ 100, 100 };//width, height
	glm::vec2 scale{ 1.f, 1.f };
	float rotation = 0.f;
	glm::vec2 getPosition();
	//Get Size Width, Height : equation size * scale;
	glm::vec2 getsize() {
		return size * scale;
	}
	glm::vec2 getLocalPosition();
};