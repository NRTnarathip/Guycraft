#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
class Bounding {
public:
	Bounding() {};
	float leftTop;
	float leftDown;
	float rightTop;
	float rightDown;
	void update(glm::vec2 position, glm::vec2 size) {
		leftDown = position.x;
		leftTop = position.y;

		rightDown = leftDown + size.x;
		rightTop = leftTop + size.y;
	}
	glm::vec2 topos() {
		return glm::vec2(leftDown, leftTop);
	}
};
class AABB {
private:
	Bounding bounding;
public:
	
	AABB() {}
	AABB(glm::vec2 pos, glm::vec2 s) {
		bounding.update(pos, s);
	}
	
	auto getbounding() const { return bounding; }

	bool isInSection(glm::vec2 pos) {
		if (bounding.rightDown >= pos.x and pos.x >= bounding.leftDown
			and bounding.rightTop >= pos.y and pos.y >= bounding.leftTop) {
			return true;
		}
		return false;
	}
};