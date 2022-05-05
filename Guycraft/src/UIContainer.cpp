#include "UIContainer.h"
#include "ResourceManager.h"
#include "UIText.h"
#include "GUI.h"

UIObject* UIContainer::createButton(std::string name) {
	auto object = createUIObject();
	object->name = name;
	auto button = object->addComponent<Button>(Button(name));
	auto image = object->addComponent<Image>(Image());
	auto uiText = object->addComponent<UIText>(UIText());
	m_buttons.push_back(button);

	//addjust position line vertical
	return object;
}
UIObject* UIContainer::createImage() {
	auto object = createUIObject();
	object->addComponent<Image>(Image());
	return object;
}
UIObject* UIContainer::createUIObject() {
	auto entity = m_registry->create();
	auto newObject = new UIObject();
	newObject->g_registry = m_registry;
	newObject->m_entity = entity;
	newObject->rect.parent = &rect;
	rect.childs.push_back(&newObject->rect);
	return newObject;
}
