#include "config.h"
#include <stdio.h>
#include <string.h>

typedef struct _listaP {
    perfil p;
    struct _listaP* proximo; 
} listaPerfil;

static const char FILE_NAME[] = "perfil.txt";
static const char TEMP[] = "temp.txt";

void cadastro(char* result);
void add_experiencia(char* result, char* email, char* exp);
void list(char* result, char* filtro, int opt);
void removePerfil(char* result, char* email);
void comando(int sockfd, char *c);
void freeLista(listaPerfil* inicio);
void perfilToString(perfil p, char *result);


/**
 * Faz o cadastro de um perfil
 * @param result - 
 */
void cadastro(char* result) {

}

/**
 * Adiciona uma experiencia a um perfil
 * @param result - string com a mensagem de retorno
 * @param email - email para adicionar a nova experiencia
 * @param exp - experiencia a ser adicionada ao perfil 
 */
void add_experiencia(char* result, char* email, char* exp) {
    FILE *perfil_file = fopen(FILE_NAME, "r");
    FILE *temp;
    listaPerfil *inicio = NULL, *perfilAtual;
    perfil aux;
    int achou = 0; // 0 - nao achou; 1 - achou

    // Le o arquivo ate achar o email pedido
    // Enquanto le o arquivo, guarda os perfils ja lidos para escrever no
    // arquivo novo modificado
    while (fread(&aux, sizeof(perfil), 1, perfil_file)) {
        if (!strcmp(aux.email, email)) {
            if (inicio == NULL) { // Guardar ponteiro inicial da lista
                inicio = malloc(sizeof(listaPerfil));
                inicio->p = aux;  
                perfilAtual = inicio;
            } else {
                perfilAtual->proximo = malloc(sizeof(listaPerfil));
                perfilAtual = perfilAtual->proximo;
                perfilAtual->p = aux;
            }
        } else {
            if (inicio != NULL) {
                perfilAtual->proximo = NULL;
            }
            strcat(aux.experiencias, ", ");
            strcat(aux.experiencias, exp);
            achou++;
            break;
        }
    }
    // Se nao achou o email, nao modifica nada
    if (!achou) {
        freeLista(inicio);
        strcat(result, "Email nao cadastrado\n");
        fclose(perfil_file);
        return;
    }
    
    temp = fopen(TEMP, "w");

    perfilAtual = inicio;
    // Escreve o que ja foi lido do arquivo anterior
    while (perfilAtual != NULL) {
        fwrite (&perfilAtual->p, sizeof(perfil), 1, temp);

        // Libera a lista ligada ao percorre-la
        inicio = perfilAtual->proximo;
        free(perfilAtual);
        perfilAtual = inicio;
    }

    // Escreve o restante no arquivo modificado
    while (fread(&aux, sizeof(perfil), 1, perfil_file)) {
        fwrite (&aux, sizeof(perfil), 1, temp);
    }

    // Muda o nome do arquivo novo para o nome do arquivo original 
    fclose(perfil_file);
    remove(FILE_NAME);
    fclose(temp);
    rename(TEMP, FILE_NAME);
    remove(TEMP);
    strcat(result, "Perfil removido com sucesso\n");

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
    int ano;
    char *h;

    // Pode ter que usar sprintf para formatar as strings
    switch (opt) {
        case LISTAR_CUR:
            while (fread(&p, sizeof(perfil), 1, perfil_file)) {
                if (strcmp(p.curso, filtro)) 
                    perfilToString(p, result);
            }
            if (strcmp(result, ""))
                strcat(result, "Nenhum perfil contem a formacao academica desejada.\n");
            break;
            
        case LISTAR_HAB:
            while (fread(&p, sizeof(perfil), 1, perfil_file)) {
                h = strtok(p.habilidades, ", ");
                while (h != NULL) {
                    if (strcmp(h, filtro)) {
                        perfilToString(p, result);
                        break;
                    }
                    h = strtok(NULL, ", ");
                }
            }
            if (strcmp(result, ""))
                strcat(result, "Nenhum perfil contem a habilidade desejada.\n");
            break;

        case LISTAR_ANO:
            ano = atoi(filtro);
            while (fread(&p, sizeof(perfil), 1, perfil_file)) {
                if (ano == p.ano_formatura) 
                    perfilToString(p, result);
            }
            if (strcmp(result, ""))
                strcat(result, "Nenhum perfil contem o ano de formacao desejado.\n");
            break;

        case LISTAR_ALL:
            while (fread(&p, sizeof(perfil), 1, perfil_file)) {
                perfilToString(p, result);
            }
            if (strcmp(result, ""))
                strcat(result, "Nenhum perfil cadastrado.\n");
            break;

        case LISTAR_EMAIL:
            while (fread(&p, sizeof(perfil), 1, perfil_file)) {
                if (strcmp(filtro, p.email)) {
                    perfilToString(p, result);
                    break;
                }
            }
            if (strcmp(result, ""))
                strcat(result, "Email nao cadastrado.\n");
            break;

    }
    fclose(perfil_file);
}

/**
 * Prepara a string para o cliente
 * @param p - perfil a ser transformado em string
 * @param result - string para concatenacao do perfil a ser transformando em string
 */
void perfilToString(perfil p, char *result) {
    char ano[7];

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
    sprintf(ano, "%d", p.ano_formatura);
    strcat(result, ano);
    strcat(result, "\nHabilidades: ");
    strcat(result, p.habilidades);
    strcat(result, "\nExperiencias: ");
    strcat(result, p.experiencias);

    strcat(result, "\n");
}


/**
 * Remove um perfil
 * @param result - string resultante da operação
 * @param email - identificador para excluir o perfil
 */
void removePerfil(char* result, char* email) {
    FILE *perfil_file = fopen(FILE_NAME, "r");
    FILE *temp;
    listaPerfil *inicio = NULL, *perfilAtual;
    perfil aux;
    int achou = 0; // 0 - nao achou; 1 - achou

    // Le o arquivo ate achar o email pedido
    // Enquanto le o arquivo, guarda os perfils ja lidos para escrever no
    // arquivo novo modificado
    while (fread(&aux, sizeof(perfil), 1, perfil_file)) {
        if (!strcmp(aux.email, email)) {
            if (inicio == NULL) { // Guardar ponteiro inicial da lista
                inicio = malloc(sizeof(listaPerfil));
                inicio->p = aux;  
                perfilAtual = inicio;
            } else {
                perfilAtual->proximo = malloc(sizeof(listaPerfil));
                perfilAtual = perfilAtual->proximo;
                perfilAtual->p = aux;
            }
        } else {
            if (inicio != NULL) {
                perfilAtual->proximo = NULL;
            }
            achou++;
            break;
        }
    }
    // Se nao achou o dado, nao modifica nada
    if (!achou) {
        freeLista(inicio);
        strcat(result, "Email nao cadastrado\n");
        return;
    }
    
    temp = fopen(TEMP, "w");

    perfilAtual = inicio;
    // Escreve o que ja foi lido do arquivo anterior
    while (perfilAtual != NULL) {
        fwrite (&perfilAtual->p, sizeof(perfil), 1, temp);
        
        // Libera a lista ligada ao percorre-la
        inicio = perfilAtual->proximo;
        free(perfilAtual);
        perfilAtual = inicio;
    }

    // Escreve o restante no arquivo modificado
    while (fread(&aux, sizeof(perfil), 1, perfil_file)) {
        fwrite (&aux, sizeof(perfil), 1, temp);
    }

    // Muda o nome do arquivo novo para o nome do arquivo original 
    remove(FILE_NAME);
    rename(TEMP, FILE_NAME);
    remove(TEMP);
    fclose(perfil_file);
}

/**
 * Libera memoria utilizada na lista ligada
 * @param inicio - inicio da lista ligada
 */
void freeLista(listaPerfil* inicio) {
    listaPerfil *atual = inicio, *prox;
    while (atual != NULL) {
        prox = atual->proximo;
        free(atual);
        atual = prox;
    }
}



/** 
 * Faz o parser do comando vindo do cliente
 * @param sockfd - socket cliente
 * @param c      - string vinda do cliente
 */
void comando(int sockfd, char *c) {
    char result [4230 * 1000];
    bzero(result, sizeof(result));

    if (strcmp(CADASTRO_CMD, c))
        cadastro(result);
    else {
        // Split string para identificar qual listamento de perfils
        // ou remocao de um perfil
        char *tk = strtok(c, " ");

        if (strcmp(ADD_EXP_CMD, tk)) {
            tk = strtok(NULL, " ");
            add_experiencia(result, tk, strtok(NULL, ""));
        } else if (strcmp(LISTAR_CUR_CMD, tk)) {
            tk = strtok(NULL, "");
            list(result, tk, LISTAR_CUR);
        } else if (strcmp(LISTAR_HAB_CMD, tk)) {
            tk = strtok(NULL, "");
            list(result, tk, LISTAR_HAB);
        } else if (strcmp(LISTAR_ANO_CMD, tk)) {
            tk = strtok(NULL, "");
            list(result, tk, LISTAR_ANO);
        } else if (strcmp(LISTAR_ALL_CMD, tk))
            list(result, NULL, LISTAR_ALL);
        else if (strcmp(LISTAR_EMAIL_CMD, tk)) {
            tk = strtok(NULL, "");
            list(result, tk, LISTAR_EMAIL);
        } else if (strcmp(REMOVER_CMD, tk)) {
            tk = strtok(NULL, "");
            removePerfil(result, tk);
        } else {
            write(sockfd, "Comando invalido\n", 17);
            return;
        }
    }
    write(sockfd, result, sizeof(result));
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
		} else
			printf("Cliente aceito\n");
    }

    return 0;
}