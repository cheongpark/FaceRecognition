#include "console/console_control.h"

void setColor(short text, short background) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text + background * 16);
}