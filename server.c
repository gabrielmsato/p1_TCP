#include "config.h"

static const char FILE_NAME[] = "perfil.txt"

void cadastro(char* result);
void add_experiencia(char* result);
void list(char* result, char* filtro, int opt);
void remove(char* result, char* email);
void comando(int sockfd, char *c);

/**
 * Faz o cadastro de um perfil
 * @param result - 
 */
void cadastro(char* result) {

}

/**
 * Adiciona uma experiencia a um perfil
 * @param result - 
 */
void add_experiencia(char* result) {
    
}


/**
 * Lista perfil/s de acordo com a opcao
 * @param result - string de retorno com a lista de perfils
 * @param filtro - string com a opcao de filtro do cliente 
 * @param opt - LISTAR_CUR      0
 *              LISTAR_HAB      1
 *              LISTAR_ANO      2
 *              LISTAR_ALL      3
 *              LISTAR_EMAIL    4
 */
void list(char* result, char* filtro, int opt) {
    FILE *perfil_file = fopen(FILE_NAME, "r");
    perfil p;
    char aux[10];

    // Pode ter que usar sprintf para formatar as strings
    switch (opt) {
        case LISTAR_CUR:
            break;
            
        case LISTAR_HAB:
            break;

        case LISTAR_ANO:
            break;

        case LISTAR_ALL:
            while (fread(p, sizeof(perfil), 1, perfil_file)) {
                strcat(result, "Email: ");
                strcat(result, p.email);
                strcat(result, "\nNome: ");
                strcat(result, p.nome);
                strcat(result, " Sobrenome: ");
                strcat(result, p.sobrenome);
                strcat(result, "\nResidencia: ");
                strcat(result, p.cidade);
                strcat(result, "\nFormacao Academica: ");
                strcat(result, p.curso);
                strcat(result, "\nAno de Formatura: ");
                sprintf(aux, "%d", ano_formatura);
                strcat(result, p.aux);
                
                // Separar aqui por ,
                strcat(result, "\nHabilidades: ");

                // Separar aqui por ()
                strcat(result, "\nExperiencias: ");

                strcat(result, "\n")
                
            }
            break;

        case LISTAR_EMAIL:
            break;

    }
    fclose(perfil_file);
}


/**
 * Remove um perfil
 * @param result - string resultante da operação
 * @param email - identificador para excluir o perfil
 */
void remove(char* result, char* email) {
    FILE *perfil_file = fopen(FILE_NAME, "r");
    perfil p;
    // Pode dar merda aqui quando tiver menos experiencias e vai ler nome por cima
    while (fread(p, sizeof(perfil), 1, perfil_file)) {
        if (strcmp(p.email, email)) {
            
        }
    }

    fclose(perfil_file);
}



/** 
 * Faz o parser do comando vindo do cliente
 * @param sockfd - socket cliente
 * @param c      - string vinda do cliente
 */
void comando(int sockfd, char *c) {
    char result [4230 * 1000];

    if (strcmp(CADASTRO_CMD, c))
        cadastro(result);
    else if (strcmp(ADD_EXP_CMD, c))
        add_experiencia(result);
    else {
        // Split string para identificar qual listamento de perfils
        // ou remocao de um perfil
        char *tk = strtok(c, " ");

        if (strcmp(LISTAR_CUR_CMD, tk)) {
            tk = strtok(NULL, " ")
            list(result, tk, LISTAR_CUR);
        } else if (strcmp(LISTAR_HAB_CMD, tk)) {
            tk = strtok(NULL, " ")
            list(result, tk, LISTAR_HAB);
        } else if (strcmp(LISTAR_ANO_CMD, tk)) {
            tk = strtok(NULL, " ")
            list(result, tk, LISTAR_ANO);
        } else if (strcmp(LISTAR_ALL_CMD, tk))
            list(result, NULL, LISTAR_ALL);
        else if (strcmp(LISTAR_EMAIL_CMD, tk)) {
            tk = strtok(NULL, " ")
            list(result, tk, LISTAR_EMAIL);
        } else if (strcmp(REMOVER_CMD, tk) {
            tk = strtok(NULL, " ")
            remove(result, tk);
        } else {
            write(sockfd, "Comando invalido\n", 17);
            return;
        }
    }
    write(sockfd, result, sizeof(result));
    bzero(result, sizeof(result));
}

int main() {
    int serverfd, fd;
    struct sockaddr_in server, client;

    // Criacao do socket 
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0) { 
        printf("Nao foi possivel a criacao do server socket\n");
        return 1;
    }
    printf("Server socket criado com sucesso\n");

    // Configurando o socket do servidor 
    server.sin_family = AF_INET; 
	server.sin_addr.s_addr = INADDR_ANY; 
	server.sin_port = htons(PORTNO); 

    // Realizando bind do server socket
    if (bind(serverfd, (struct sockaddr *) &server , sizeof(server)) < 0) {
        printf("Nao foi possivel fazer o bind do server socket\n");
        return 1;
    }
    printf("Bind feito com sucesso\n");

    // Listen servidor e testagem
	if ((listen(serverfd, 5)) != 0) { 
		printf("Erro no server socket listen\n"); 
		return 1; 
	} 
	else
		printf("Server socket listen efetuado com sucesso\n"); 

    // Loop do server ate ele ser fechado
    while (1) {
        // Aceita pacote vindo de um cliente 
        fd = accept(serverfd, (struct sockaddr *)&client, sizeof(client));
        
        // Verificacao do que recebeu
        if (fd < 0) { 
			printf("Cliente nao foi aceito\n"); 
			return 1; 
		} 
		else
			printf("Cliente aceito\n");
    }

    return 0;
}