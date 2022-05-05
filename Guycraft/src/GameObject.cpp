#include <GameObject.h>
#include <World.h>
GameObject::GameObject() {
	g_registry = &World::GetRegistry();
}
Transform* GameObject::getChildIndex(int index) const {
	if (index >= childs.size()) {
		return nullptr;
	}
	return childs[index];
};