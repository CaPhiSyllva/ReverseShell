// WinSockManager.h
#pragma once
#include "WinAPIException.h"

class WinSockManager {
public:
    WinSockManager();
    ~WinSockManager();

    WinSockManager(const WinSockManager&) = delete;
    WinSockManager& operator=(const WinSockManager&) = delete;
};
