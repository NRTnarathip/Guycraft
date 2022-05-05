#pragma once
#include <iostream>

class UIObject;

class UIComponent {
public:
	UIObject* m_uiObject = nullptr;
	virtual void updateEventInput() {
		printf("update event input base class\n");
	};
	virtual void init() {}
	virtual void update() {};
	virtual void render() {};
};