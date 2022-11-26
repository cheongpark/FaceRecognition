#include "console/console_control.h"

void setColor(short textColor, short background) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textColor + background * 16);
}

void ErrorExit() {
	system("pause");
	exit(1);
}