#ifndef SINGLECHARACTER_H
#define SINGLECHARACTER_H

#include <iostream>
#ifdef _WIN32
#include <conio.h>
#define OS_NAME "Windows"
#define CLEAR "cls"
#elif __APPLE__
#include <TargetConditionals.h>
#include <termios.h>
#include <unistd.h>
#if TARGET_OS_MAC
#define OS_NAME "macOS"
#define CLEAR "clear"
#endif
#else // Linux and other Unix-like systems
#include <termios.h>
#include <unistd.h>
#define OS_NAME "Linux"
#define CLEAR "clear"
#endif

char getSingleChar() {
#ifdef _WIN32
    return _getch();
#else // macOS and Linux both use termios
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

#endif // SINGLECHARACTER_H