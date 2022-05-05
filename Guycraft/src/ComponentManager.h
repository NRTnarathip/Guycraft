#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H
#include <vector>
class Component;

class ComponentManager {
public:
	ComponentManager();
	static ComponentManager* instance;
	static ComponentManager* GetInstance() { return instance; }

	std::vector<Component*> components;
};
#endif
