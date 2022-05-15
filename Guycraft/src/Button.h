#pragma once
#include "SpriteRenderer.h"
#include "Physics/Transform.h"
#include "AABB.h"
#include <glm/vec4.hpp>
#include "RectTransform.h"
#include "Image.h"
#include "UIComponent.h"
#include <vector>

class Button : public UIComponent {
private:
	void onHover();
	void outHover();
	bool isOnHover = false;
public:
	typedef void(*fnPtr)();
	fnPtr m_fnOnClick = nullptr;

	class ColorTransition {
	public:
		glm::vec4 colorHover;
		glm::vec4 colorNormal;
		glm::vec4 colorPress;
	};
	Button() {}
	Button(std::string name) {
		this->name = name;
	}

	std::string title = "Emty Title";
	std::string name = "emty_button";
	glm::vec4 color{ 1.f,1.f,1.f,1.f };
	ColorTransition colors;
	void init();
	void updateEventInput();
};