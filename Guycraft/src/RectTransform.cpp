#include "RectTransform.h"
#include "UIObject.h"
//is useonly just UIContainer as parent and UICompoent as child
glm::vec2 RectTransform::inceptGetPosition() {
	auto pos = getLocalPosition();
	if (parent) {
		pos += parent->inceptGetPosition();
	}
	return pos;
}
glm::vec2 RectTransform::getPosition() {
	auto pos = anchorPreset * glm::vec2(1920, 1080);
	pos += inceptGetPosition();
	return pos;
}
glm::vec2 RectTransform::getLocalPosition()
{
	return position;
}
