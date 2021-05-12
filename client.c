#include "config.h"

// leitura de comnados do usuário e envio de pacote para o server
void comunicacao(int sockFD) { 
    char buff[2000];
	int inicio =  1;
	bzero(buff, sizeof(buff)); 
    while(1) {
        // Pede comando de uma lista
        printf("Escreva um dos comandos a seguir para ser realizado:\n%s: para todos os perfis e informacoes\n%s: para todos os perfis de formandos de um ano\n%s: para todos os perfis de formandos de um curso\n%s: para todos os perfis com uma habilidade\n%s: mostra todas as informações de um perfil relacionado a um email\n%s: para adicionar uma experiencia em um perfil\n%s: para remover um perfil\n%s: para adicionar um perfil\n%s: para sair\n\n",LISTAR_ALL_CMD,LISTAR_ANO_CMD,LISTAR_CUR_CMD,LISTAR_HAB_CMD,LISTAR_EMAIL_CMD,ADD_EXP_CMD,REMOVER_CMD,CADASTRO_CMD,SAIR_CMD); 
        scanf(" %s", buff);
        // manda comando para o server
        write(sockFD, buff, sizeof(buff));
        // se o comando for cadastro o server vai mandando o que é para ser impresso para o usuario dar o input ate envviar uma flag de fim de cadastro
        if (!strcmp(buff, CADASTRO_CMD)) {
            bzero(buff, sizeof(buff));
            read(sockFD, buff, sizeof(buff));
            while (strcmp(buff, FLAG_FINAL)) {
                printf("%s\n", buff);
                bzero(buff, sizeof(buff));
                scanf(" %s", buff);
                write(sockFD, buff, sizeof(buff));
                bzero(buff, sizeof(buff));
                read(sockFD, buff, sizeof(buff));
            }
        } else {
            // senao o server so envia o resultado para ser impresso
            bzero(buff, sizeof(buff));
            read(sockFD, buff, sizeof(buff));
            printf("%s\n", buff);
        }
    } 
}

int main() { 
	int sockFD; 
	struct sockaddr_in SA; 

	// Cria o socket e verificca se a criacao ocorreu
	sockFD = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockFD != -1) { 
        printf("A criacao do socket foi bem sucedida!\n"); 
	} else {
        printf("A criacao do socket falhou\n"); 
		exit(0); 
    }

	// atribui o ip e a porta
	bzero(&SA, sizeof(SA)); 
    SA.sin_family = AF_INET; 
	SA.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	SA.sin_port = htons(PORTNO); 

	// conecta o cliente com o servidor
	if (connect(sockFD, (sockAddr*)&SA, sizeof(SA)) == 0) { 
        printf("Conectado ao server\n"); 
	} else {
		printf("A conexao nao pode ser estabelecida.\n"); 
		exit(0); 
    }

	// funcao de comunicação
	comunicacao(sockFD); 

	// fecha conexao com o socket
	close(sockFD); 
} 
