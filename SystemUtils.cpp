// SystemUtils.cpp
#include "SystemUtils.h"
#include <windows.h>

void SystemUtils::hideConsole() {
    if (AllocConsole()) {
        if (HWND consoleWindow = FindWindowA("ConsoleWindowClass", nullptr)) {
            ShowWindow(consoleWindow, SW_HIDE);
        }
    }
}
