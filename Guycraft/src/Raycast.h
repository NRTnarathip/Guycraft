#pragma once
#include <glm/vec3.hpp>
using namespace glm;

class Raycast {
public:
	vec3 raycastBlock(vec3 playerPos, vec3 playerLookAt);
};