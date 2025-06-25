// SocketHandler.cpp
#include "SocketHandler.h"

SocketHandler::SocketHandler(SOCKET s) : socket_(s) {}

SocketHandler::~SocketHandler() {
    if (socket_ != INVALID_SOCKET) {
        closesocket(socket_);
    }
}

SocketHandler::SocketHandler(SocketHandler&& other) noexcept : socket_(other.socket_) {
    other.socket_ = INVALID_SOCKET;
}

SocketHandler& SocketHandler::operator=(SocketHandler&& other) noexcept {
    if (this != &other) {
        socket_ = other.socket_;
        other.socket_ = INVALID_SOCKET;
    }
    return *this;
}

SocketHandler::operator SOCKET() const { 
    return socket_; 
}

void SocketHandler::connect(const sockaddr_in& address) {
    if (::connect(socket_, reinterpret_cast<const SOCKADDR*>(&address), 
                 sizeof(address)) == SOCKET_ERROR) {
        throw WinAPIException("Connection failed");
    }
}

int SocketHandler::receive(char* buffer, int bufferSize) {
    int bytesReceived = recv(socket_, buffer, bufferSize - 1, 0);
    if (bytesReceived == SOCKET_ERROR) {
        throw WinAPIException("Receive failed");
    }
    return bytesReceived;
}

void SocketHandler::send(const char* data, int dataSize) {
    if (::send(socket_, data, dataSize, 0) == SOCKET_ERROR) {
        throw WinAPIException("Send failed");
    }
}
