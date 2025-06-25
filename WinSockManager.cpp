// WinSockManager.cpp
#include "WinSockManager.h"
#include <winsock2.h>

WinSockManager::WinSockManager() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw WinAPIException("WSAStartup failed");
    }
}

WinSockManager::~WinSockManager() {
    WSACleanup();
}
