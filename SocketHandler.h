// SocketHandler.h
#pragma once
#include "WinAPIException.h"
#include <winsock2.h>

class SocketHandler {
    SOCKET socket_;
    
public:
    explicit SocketHandler(SOCKET s = INVALID_SOCKET);
    ~SocketHandler();

    SocketHandler(const SocketHandler&) = delete;
    SocketHandler& operator=(const SocketHandler&) = delete;
    
    SocketHandler(SocketHandler&& other) noexcept;
    SocketHandler& operator=(SocketHandler&& other) noexcept;
    
    operator SOCKET() const;
    
    void connect(const sockaddr_in& address);
    int receive(char* buffer, int bufferSize);
    void send(const char* data, int dataSize);
};
