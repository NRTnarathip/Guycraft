#pragma once
#include <iostream>
typedef uint8_t blockdata_t;
typedef uint16_t blocktype_t;

struct Block {
	blocktype_t type;
	blockdata_t data;
};