// ProcessInjector.h
#pragma once
#include <windows.h>
#include <string>

class ProcessInjector {
public:
    static bool inject(DWORD pid, const std::string& dllPath);

private:
    struct ScopedHandle;
    struct ScopedRemoteMemory;
};
