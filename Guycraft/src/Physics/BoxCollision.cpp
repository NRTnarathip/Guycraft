#include "BoxCollision.h"
#include "ClientEngine.h"

bool isSection(float aMin, float aMax, float bMin, float bMax) {
	return (aMax >= bMin and bMax >= aMin);
}
bool BoxCollision::isCollider(BoxCollision other) {
	auto otherPosition = other.m_position;
	auto otherDimension = other.m_dimension;
	auto otherDimensionHalf = other.m_dimensionHalf;

	auto a_min = m_position - m_dimensionHalf;
	auto a_max = m_position + m_dimensionHalf;
	auto b_min = otherPosition - otherDimensionHalf;
	auto b_max = otherPosition + otherDimensionHalf;

	if (a_max.x >= b_min.x and b_max.x >= a_min.x
		and a_max.y >= b_min.y and b_max.y >= a_min.y
		and a_max.z >= b_min.z and b_max.z >= a_min.z){
		return true;
	}
	/*if (isSection(a_min.x, a_max.x, b_min.x, b_max.x) and
		isSection(a_min.y, a_max.y, b_min.y, b_max.y) and
		isSection(a_min.z, a_max.z, b_min.z, b_max.z)) {
		return true;
	}*/

	return false;
}

void BoxCollision::beforeUpdate() {
	auto me = getGameObject();
	m_position = me->transform.position;
}
void BoxCollision::update() {

}
void BoxCollision::draw() {
	
}