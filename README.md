# Reverse Shell em C

Este repositório contém um exemplo educativo de uma shell reversa implementada em C, utilizando a biblioteca Winsock no Windows. O objetivo é demonstrar conceitos de comunicação de rede e execução remota de comandos, úteis para profissionais de segurança da informação.

## Índice

- [Descrição do Projeto](#descrição-do-projeto)
- [Funcionalidades](#funcionalidades)
- [Arquitetura do Código](#arquitetura-do-código)
- [Requisitos](#requisitos)
- [Como Compilar e Executar](#como-compilar-e-executar)
- [Considerações de Segurança](#considerações-de-segurança)
- [Contribuições](#contribuições)
- [Licença](#licença)

## Descrição do Projeto

Este código implementa uma shell reversa que conecta a um servidor remoto, permitindo a execução de comandos no sistema alvo e o envio da saída de volta para o servidor. Este projeto é uma ferramenta de aprendizado sobre como ataques de shell reversa funcionam e as implicações de segurança associadas.

## Funcionalidades

- **Conexão TCP**: Estabelece uma conexão TCP com um servidor remoto definido pelo IP e porta.
- **Execução de Comandos**: Recebe comandos do servidor, executa-os no sistema local e captura a saída.
- **Comunicação**: Envia a saída dos comandos de volta ao servidor.
- **Console Oculto**: A janela do console é oculta para evitar a detecção visual da execução.

## Arquitetura do Código

O código é estruturado em duas funções principais:

1. **`execute_command(char* output_buffer, int buffer_size, const char *command)`**:
   - Executa um comando no sistema e armazena a saída em um buffer.

2. **`start_reverse_shell()`**:
   - Inicializa a comunicação de rede, aguarda comandos do servidor e utiliza `execute_command` para processá-los.

### Fluxo do Programa

1. O console é alocado e ocultado.
2. A conexão com o servidor é estabelecida.
3. Um loop aguarda comandos do servidor.
4. Os comandos são executados e as saídas são enviadas de volta.

## Requisitos

- Compilador C/C++ compatível com Windows (ex: GCC, Visual Studio).
- Biblioteca Winsock (normalmente incluída no ambiente de desenvolvimento do Windows).

## Como Compilar e Executar

### Compilação

1. **Abra o terminal ou prompt de comando.**
2. **Navegue até o diretório onde o código está salvo.**
3. **Compile o código usando um compilador C:**
   ```bash
   gcc -o reverse_shell reverse_shell.c -lws2_32
   ```

### Execução

- Execute o programa compilado:
  ```bash
  ./reverse_shell
  ```

**Nota**: O endereço IP e a porta do servidor devem ser configurados no código antes da execução.

## Considerações de Segurança

Este projeto é destinado a fins educacionais. O uso de shells reversas em ambientes não autorizados é ilegal e antiético. Sempre obtenha permissão explícita antes de realizar testes de segurança em qualquer sistema.

### Práticas Recomendadas

- Utilize ambientes de teste isolados.
- Estude e implemente técnicas de defesa contra shells reversas.
- Mantenha-se atualizado sobre as melhores práticas em segurança cibernética.

## Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para abrir issues ou pull requests.

## Licença

Este projeto é licenciado sob a [MIT License](LICENSE). Use-o como desejar, mas sempre com responsabilidade.

---

**Atenção**: Este código não deve ser usado para fins maliciosos. O conhecimento adquirido deve ser aplicado de maneira ética e responsável.
