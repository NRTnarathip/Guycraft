#pragma once
#ifndef UICONTAINER_H
#define UICONTAINER_H

#include "RectTransform.h"
#include <string>
#include <map>
#include <vector>
#include "Button.h"
#include "SpriteRenderer.h"
#include "UIObject.h"
#include "Image.h"
#include "entt/entt.hpp"
#include "UIText.h"


class UIContainer {
public:
	UIContainer(std::string containerName,glm::vec2 pos, glm::vec2 size) : name(containerName){
		rect.size = size;
		rect.position = pos;
	}
	entt::registry* m_registry = nullptr;
	std::string name;
	RectTransform rect;
	std::vector<Button*> m_buttons;
	
	UIObject* createUIObject();
	UIObject* createButton(std::string name);
	UIObject* createImage();
};
#endif // !UICONTAINER_H
