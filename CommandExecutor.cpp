// CommandExecutor.cpp
#include "CommandExecutor.h"
#include <cstdio>
#include <memory>

std::string CommandExecutor::execute(const std::string& command) {
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
    if (!pipe) {
        throw WinAPIException("Failed to execute command");
    }

    std::string result;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }

    return result;
}
