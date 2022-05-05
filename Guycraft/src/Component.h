#pragma once
class GameObject;

class Component {
private:
	GameObject* m_gameObject;
public:
	GameObject* getGameObject() { return m_gameObject; }
	void setupComponent(GameObject* refObject) { m_gameObject = refObject; }
	virtual void init() {};
	virtual void beforeUpdate() {};
	virtual void update() {};
	virtual void lastUpdate() {};
	virtual void draw() {};
};