#include "utils.h"
#include <conio.h>
#include<stdio.h>

void setColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
