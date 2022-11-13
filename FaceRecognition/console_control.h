#pragma once
#include <Windows.h>

enum COLOR {
	BLACK = 0,
	DARK_BLUE = 1,
	DARK_GREEN = 2,
	DARK_AQUA = 3,
	DARK_RED = 4,
	DARK_PURPLE = 5,
	DARK_YELLOW = 6,
	DARK_WHITE = 7,
	GRAY = 8,
	BLUE = 9,
	GREEN = 10,
	AQUA = 11,
	RED = 12,
	PURPLE = 13,
	YELLOW = 14,
	WHITE = 15
};

void setColor(short text = COLOR::DARK_WHITE, short background = COLOR::BLACK);