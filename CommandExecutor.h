// CommandExecutor.h
#pragma once
#include "WinAPIException.h"
#include <string>

class CommandExecutor {
public:
    static std::string execute(const std::string& command);
};
