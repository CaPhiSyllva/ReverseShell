// WinAPIException.h
#pragma once
#include <stdexcept>
#include <string>
#include <windows.h>

class WinAPIException : public std::runtime_error {
public:
    WinAPIException(const std::string& message, DWORD errorCode = GetLastError());
};
