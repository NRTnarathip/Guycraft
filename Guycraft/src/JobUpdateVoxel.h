#pragma once
#include "Voxel.h"
#include "glm/glm.hpp"

struct JobUpdateVoxel { 
	Voxel voxel;
	glm::ivec3 worldPos;
};