#pragma once
#include <BlockSystem/Block.h>
#include "glm/glm.hpp"

struct JobUpdateBlock { 
	Block block;
	glm::ivec3 worldPos;
};