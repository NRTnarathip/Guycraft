#include "UIObject.h"
void UIObject::attachObject(UIObject* child) {
	rect.childs.push_back(&child->rect);
	child->rect.parent = &rect;
}
void UIObject::render() {
	for (auto comp : m_components) {
		comp->render();
	}
	for (auto child : rect.childs) {
		child->m_uiObject->render();
	}
}
void UIObject::update() {
	for (auto c : m_components)
		c->update();
}
void UIObject::swapOrderComponent(UIComponent* from, UIComponent* to) {
	auto indexFrom = 0;
	auto indexTo = 1;
	{
		auto it = std::find(m_components.begin(), m_components.end(), from);
		// If element was found
		if (it != m_components.end())
		{
			// calculating the index
			// of K
			indexFrom = it - m_components.begin();
		}

		it = std::find(m_components.begin(), m_components.end(), to);
		// If element was found
		if (it != m_components.end())
		{
			// calculating the index
			// of K
			indexTo = it - m_components.begin();
		}
	}
	m_components[indexFrom] = to;
	m_components[indexTo] = from;
}