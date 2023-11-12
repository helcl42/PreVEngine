#include "Logger.h"

void SetConsoleTextColor(const ConsoleColor color)
{
#ifdef _WIN32
    const char bgr[] = { 7, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15 }; // RGB-to-BGR
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, bgr[static_cast<uint32_t>(color)]);
#elif __LINUX__
    if (color == ConsoleColor::FAINT) {
        printf("\033[37m\033[2m"); // set faint white
        return;
    }
    printf("\033[%dm", (int(color) & 8) ? 1 : 0); // bright or normal
    if (bool(color)) {
        printf("\033[3%dm", int(color) & 7); // set text color
    }
#endif
}