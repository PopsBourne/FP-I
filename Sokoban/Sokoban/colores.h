#pragma once
#include <windows.h>

enum Color {
	Gris, Rojo, Verde, Azul, Blanco = 15
};

#ifdef _WIN32

void setColor(int color) {
	WORD a = 0;
	switch (color) {
	case Gris: a = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN; break;
	case Rojo: a = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
	case Verde: a = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
	case Azul: a = FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
	default: a = color;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a);
}


#endif

