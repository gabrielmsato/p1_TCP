#include "config.h"

int main() {
    int serverfd;
    struct sockaddr_in server

    // Criacao do socket 
    serverfd = socket(AF_INET, SOCK_STREAM, 0)
    if (serverfd < 0) { 
        printf("Nao foi possivel a criacao do server socket\n");
        return 1;
    }
    printf("Server socket criado com sucesso");

    // Configurando o socketdo servidor 
    server.sin_family = AF_INET; 
	server.sin_addr.s_addr = INADDR_ANY; 
	server.sin_port = htons(PORTNO); 
    return 0;
}