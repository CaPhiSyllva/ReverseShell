// ProcessInjector.cpp
#include "ProcessInjector.h"

// Implementação de ScopedHandle
struct ProcessInjector::ScopedHandle {
    HANDLE handle;
    
    explicit ScopedHandle(HANDLE h = nullptr) : handle(h) {}
    ~ScopedHandle() { if (handle) CloseHandle(handle); }
    
    operator HANDLE() const { return handle; }
    
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;
    
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

// Implementação de ScopedRemoteMemory
struct ProcessInjector::ScopedRemoteMemory {
    HANDLE hProcess;
    LPVOID memory;
    
    ScopedRemoteMemory(HANDLE hProc, LPVOID mem) : hProcess(hProc), memory(mem) {}
    ~ScopedRemoteMemory() { 
        if (memory) VirtualFreeEx(hProcess, memory, 0, MEM_RELEASE); 
    }
    
    operator LPVOID() const { return memory; }
    
    ScopedRemoteMemory(const ScopedRemoteMemory&) = delete;
    ScopedRemoteMemory& operator=(const ScopedRemoteMemory&) = delete;
    
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

// Implementação da injeção
bool ProcessInjector::inject(DWORD pid, const std::string& dllPath) {
    ScopedHandle hProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid));
    if (!hProcess) return false;

    ScopedRemoteMemory pDllPath(
        hProcess.get(), 
        VirtualAllocEx(hProcess.get(), nullptr, dllPath.size() + 1, 
                      MEM_COMMIT, PAGE_READWRITE)
    );
    if (!pDllPath) return false;

    if (!WriteProcessMemory(hProcess.get(), pDllPath.get(), dllPath.c_str(), 
                          dllPath.size() + 1, nullptr)) {
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleA("Kernel32");
    auto loadLibraryFunc = reinterpret_cast<LPTHREAD_START_ROUTINE>(
        GetProcAddress(hKernel32, "LoadLibraryA")
    );

    ScopedHandle hThread(CreateRemoteThread(
        hProcess.get(), nullptr, 0, loadLibraryFunc, 
        pDllPath.get(), 0, nullptr
    ));
    if (!hThread) return false;

    WaitForSingleObject(hThread.get(), INFINITE);
    return true;
}
