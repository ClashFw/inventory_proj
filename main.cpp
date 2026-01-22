#include <iostream>
#ifdef _WIN32
#include <conio.h> // For _getch() on Windows
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
#else
#define OS_NAME "Unknown OS"
#define CLEAR "clear"
#endif

char getSingleChar() {
#ifdef _WIN32
    return _getch(); // Windows: Get character without Enter
#elif __APPLE__
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt); // Get current terminal attributes
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable line buffering & echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply new attributes
    ch = getchar(); // Read single character
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore old attributes
    return ch;
#endif
}

void performAction() {
    std::cout << "Press any key: ";
    char ch = getSingleChar(); // Get single key press
    system(CLEAR);
    std::cout << "\nYou pressed: " << ch << std::endl;
}

int main() {
    std::cout << "Detected OS: " << OS_NAME << std::endl;
    performAction();
    return 0;
}
