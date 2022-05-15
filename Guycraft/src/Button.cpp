#include "Button.h"
#include "Input.h"
#include "Physics/BoxCollision.h"
#include "Game.h"
void Button::init() {
	colors.colorHover = glm::vec4(0.85f, 0.85f, 0.85f, 1.f);
	colors.colorPress = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
	colors.colorNormal = glm::vec4(1.f);
}
void Button::updateEventInput()
{
	auto rect = m_uiObject->rect;
	auto aabb = AABB(rect.getPosition() - rect.getoffs(), rect.getsize());

	auto& input = Input::GetInstance();
	auto mousePos = input.getMousePosition();
	bool isMouseHover = aabb.isInSection(mousePos);

	if (isMouseHover) {
		if (input.onMouseDown(0)) {
			if (m_fnOnClick != nullptr) 
				m_fnOnClick();
		}

		if (input.onMouse(0)) {
			//on mouse press hold
			color = colors.colorPress;
		}
		else if (input.onMouseUp(0)) {
			color = colors.colorNormal;
		}  


		if (isOnHover == false) {
			isOnHover = true;
			//color = colors.colorHover;
			onHover();
		}
	}
	else {
		if (isOnHover) {
			isOnHover = false;
			color = colors.colorNormal;

			outHover();
		}
	}

	//update color via component image
	auto image = m_uiObject->getComponent<Image>();
	image->color = color;
}
void Button::outHover() {
	m_uiObject->rect.scale = glm::vec2(1.f);
}
void Button::onHover() {
	m_uiObject->rect.scale = glm::vec2(1.2f);
}