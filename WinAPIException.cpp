// WinAPIException.cpp
#include "WinAPIException.h"

WinAPIException::WinAPIException(const std::string& message, DWORD errorCode)
    : std::runtime_error(message + " (Error: " + std::to_string(errorCode) + ")") {}
