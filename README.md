# Projeto Reverse Shell - Documentação Técnica

## Visão Geral
Este projeto implementa um cliente de reverse shell modular para sistemas Windows usando princípios modernos de design em C++. A aplicação estabelece uma conexão com um servidor de comando e controle (C2), executa comandos recebidos e retorna a saída. Projetado para fins educacionais, esta implementação demonstra práticas seguras de codificação, gerenciamento de recursos e técnicas de segurança ofensiva.

**Aviso**: Esta ferramenta destina-se apenas a fins educacionais e testes de penetração autorizados. O uso não autorizado é ilegal.

## Características Principais
- **Arquitetura modular** com separação de responsabilidades
- **Gerenciamento de recursos baseado em RAII** para sockets e handles
- **Tratamento seguro de exceções** com relatórios de erro detalhados
- **Capacidade de injeção de processos** para injeção de DLL
- **Ocultação de console** para operação discreta
- **Comunicação baseada em TCP** com execução de comandos
- **Propagação de erros entre componentes** via exceções personalizadas

## Arquitetura Técnica

### Diagrama de Componentes
```
+----------------+       +-----------------+       +---------------+
|   main.cpp     |------>|  ReverseShell   |<------| CommandExecutor
+----------------+       +-----------------+       +---------------+
        |                   |           |
        |                   |           |
        v                   v           v
+----------------+   +-----------------+   +---------------+
| SystemUtils    |   |  SocketHandler  |   | ProcessInjector
+----------------+   +-----------------+   +---------------+
        |                   |
        |                   |
        v                   v
+----------------+   +-----------------+
| WinAPIException|   | WinSockManager  |
+----------------+   +-----------------+
```

### Componentes Principais
1. **WinAPIException** - Classe de exceção personalizada para erros da API Windows
2. **WinSockManager** - Wrapper RAII para inicialização/limpeza do Winsock
3. **SocketHandler** - Gerenciamento seguro de sockets com semântica de movimento
4. **CommandExecutor** - Interface segura para execução de comandos
5. **ProcessInjector** - Injeção de DLL com gerenciamento de recursos com escopo
6. **ReverseShell** - Lógica central da reverse shell
7. **SystemUtils** - Utilitários de manipulação do sistema

## Dependências
- Windows SDK (versão 10.0+)
- Winsock 2.2 (ws2_32.lib)
- Compilador compatível com C++17
- Bibliotecas da API Windows

## Instruções de Compilação

### Requisitos do Compilador
- MinGW-w64 (g++ 9.0+)
- MSVC (Visual Studio 2019+)

### Compilação com MinGW
```bash
g++ -o reverse_shell.exe main.cpp WinAPIException.cpp WinSockManager.cpp \
    SocketHandler.cpp CommandExecutor.cpp ProcessInjector.cpp \
    ReverseShell.cpp SystemUtils.cpp -lws2_32 -static -O2 -s
```

### Compilação com MSVC
```cmd
cl /EHsc /O2 /std:c++17 /Fe:reverse_shell.exe main.cpp WinAPIException.cpp \
   WinSockManager.cpp SocketHandler.cpp CommandExecutor.cpp \
   ProcessInjector.cpp ReverseShell.cpp SystemUtils.cpp ws2_32.lib
```

## Configuração
Modifique `ReverseShell.h` para configurar os parâmetros de conexão:
```cpp
static constexpr int DEFAULT_PORT = 4444;       // Porta do servidor
const char* DEFAULT_IP = "192.168.1.100";       // IP do servidor
static constexpr int DEFAULT_BUFLEN = 4096;     // Tamanho do buffer
```

## Utilização
1. Inicie um listener na máquina do atacante:
   ```bash
   nc -lvp 4444
   ```
2. Execute o binário compilado no sistema alvo
3. Envie comandos através do listener:
   ```
   > whoami /all
   > systeminfo
   > net user
   ```

## Estrutura de Arquivos
```
reverse-shell/
├── include/               # Cabeçalhos públicos
│   ├── CommandExecutor.h
│   ├── ProcessInjector.h
│   ├── ReverseShell.h
│   ├── SocketHandler.h
│   ├── SystemUtils.h
│   ├── WinAPIException.h
│   └── WinSockManager.h
├── src/                   # Arquivos de implementação
│   ├── CommandExecutor.cpp
│   ├── ProcessInjector.cpp
│   ├── ReverseShell.cpp
│   ├── SocketHandler.cpp
│   ├── SystemUtils.cpp
│   ├── WinAPIException.cpp
│   └── WinSockManager.cpp
├── main.cpp               # Ponto de entrada
└── CMakeLists.txt         # Configuração de build (opcional)
```

## Considerações de Segurança

### Medidas Protetivas
1. **Validação de Entrada**: Toda entrada de rede é limitada por buffer
2. **Encapsulamento de Recursos**: Padrão RAII previne vazamentos
3. **Isolamento de Erros**: Tratamento de erros específico por componente
4. **Segurança de Memória**: Buffers baseados em vetor previnem overflows

### Diretrizes Éticas
1. **Autorização Explícita**: Obtenha permissão por escrito antes de testar
2. **Ambientes Controlados**: Use redes isoladas e máquinas virtuais
3. **Manuseio de Dados**: Evite informações sensíveis durante os testes
4. **Divulgação Responsável**: Reporte vulnerabilidades descobertas de forma responsável

### Evasão de Detecção (Para Pesquisa)
1. **Evasão de Análise Estática**:
   - Ofuscação de strings
   - Aplanamento de fluxo de controle
2. **Evasão Comportamental**:
   - Indireção de chamadas de API
   - Técnicas de ofuscação de sleep
3. **Evasão de Rede**:
   - Criptografia SSL/TLS
   - Mimetismo de protocolo

## Anti-Padrões a Evitar
```cpp
// INSEGURO: Ponteiros crus sem RAII
SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
// ... código ...
closesocket(s); // Possível vazamento se ocorrer exceção

// SEGURO: Uso de wrapper RAII
SocketHandler safe_socket(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
```

## Extensão de Funcionalidades
### Adicionando Recursos
1. **Módulo de Persistência**:
   ```cpp
   class PersistenceManager {
   public:
       static void install();
       static void uninstall();
   };
   ```
2. **Transferência de Arquivos**:
   ```cpp
   class FileTransfer {
   public:
       static void upload(SocketHandler& socket, const std::string& path);
       static void download(SocketHandler& socket, const std::string& path);
   };
   ```
3. **Comunicação Criptografada**:
   ```cpp
   class SecureChannel {
   public:
       void encrypt(BYTE* data, size_t size);
       void decrypt(BYTE* data, size_t size);
   };
   ```

## Licença
Este projeto educacional é licenciado sob a **Academic Free License v3.0**:
- Permite uso educacional e de pesquisa
- Proíbe uso comercial e malicioso
- Exige atribuição
- Não inclui garantias

## Contribuição
1. Reporte problemas no rastreador de issues
2. Envie PRs para o branch de desenvolvimento
3. Siga as diretrizes de codificação segura
4. Inclua testes abrangentes
5. Documente todas as novas funcionalidades

> **Aviso Legal**: Este projeto demonstra conceitos de segurança para fins defensivos. Os autores não se responsabilizam por uso não autorizado ou malicioso. Sempre cumpra as leis locais e obtenha autorização adequada antes de usar qualquer ferramenta de segurança.
