#include "config.h"
#include <pthread.h>

typedef struct _listaP {
    perfil p;
    struct _listaP* proximo; 
} listaPerfil;

static const char FILE_NAME[] = "perfil.txt";
static const char TEMP[] = "temp.txt";

void cadastro(int sock, char* result);
void add_experiencia(char* result, char* email, char* exp);
void list(char* result, char* filtro, int opt);
void removePerfil(char* result, char* email);
void comando(int sock, char* msg, char* result);
void freeLista(listaPerfil* inicio);
void perfilToString(perfil p, char *result);
void *recebeCliente(void *sockfd);

// Lock para threads nao acessarem arquivo ao mesmo tempo
pthread_mutex_t lock;


/**
 * Faz o cadastro de um perfil
 * @param sock - cliente
 * @param result - string de retorno para o cliente
 */
void cadastro(int sock, char* result) {
    FILE *perfil_file;
    int msgSize, habLen = 0, expLen = 0;
    char msg[2000];
    perfil novoPerfil;
    char strQtdExp[7];

    bzero(msg, sizeof(msg));
    write(sock, "Email: ", strlen("Email: "));
    sleep(1);
    read(sock, msg, sizeof(msg));
    if (strlen(msg) > 0)
        strcat(novoPerfil.email, msg);
    bzero(msg, sizeof(msg));

    write(sock, "\nNome: ", strlen("\nNome: "));
    sleep(1);
    read(sock, msg, sizeof(msg));
    if (strlen(msg) > 0)
        strcat(novoPerfil.nome, msg);
    bzero(msg, sizeof(msg));

    write(sock, "\nSobrenome: ", strlen("\nSobrenome: "));
    sleep(1);
    read(sock, msg, sizeof(msg));
    if (strlen(msg) > 0)
        strcat(novoPerfil.sobrenome, msg);
    bzero(msg, sizeof(msg));

    write(sock, "\nCidade: ", strlen("\nCidade: "));
    sleep(1);
    read(sock, msg, sizeof(msg));
    if (strlen(msg) > 0)
        strcat(novoPerfil.cidade, msg);
    bzero(msg, sizeof(msg));

    write(sock, "\nCurso: ", strlen("\nCurso: "));
    sleep(1);
    read(sock, msg, sizeof(msg));
    if (strlen(msg) > 0)
        strcat(novoPerfil.curso, msg);
    bzero(msg, sizeof(msg));

    write(sock, "\nAno de Formatura: ", strlen("\nAno de Formatura: "));
    sleep(1);
    read(sock, msg, sizeof(msg));
    if (strlen(msg) > 0)
        novoPerfil.ano_formatura = atoi(msg);


    // Fazer ele inserir corretamente
    while (1) {
        bzero(msg, sizeof(msg)); 
        strcat(msg, "\nDigite uma habilidade e pressione [ENTER] (digite \"");
        strcat(msg, FIM_INSERCAO_HAB_EXP);
        strcat(msg, "\" para finalizar a insercao de habilidades): ");
        write (sock, msg, sizeof(msg));
        sleep(1);

        bzero(msg, sizeof(msg));
        read(sock, msg, sizeof(msg));
        printf("%s %d\n",msg,strlen(msg));
        // msgSize = recv(sock , msg , 2000 , 0);
        // msg[msgSize] = '\0'; // Finalizando string recebida
        if (!strcmp(msg, FIM_INSERCAO_HAB_EXP)) {
            printf("Entrou aqui\n");
            break;
        }
        msgSize = strlen(msg);
        habLen += msgSize;
        if (habLen >= MAX_HABILIDADES) {
            strcat(msg, "\nNumero maximo de habilidades excedido");
            break;
        }
        
        if (habLen == msgSize)
            strcat(novoPerfil.habilidades, msg);
        else {
            strcat(novoPerfil.habilidades, ", ");
            strcat(novoPerfil.habilidades, msg);
        }
        habLen += 2;
    }

    bzero(msg, sizeof(msg));
    novoPerfil.qtdExp = 0;
    while (1) {
        strcat(msg, "\nDigite uma experiencia e pressione [ENTER] (digite \"");
        strcat(msg, FIM_INSERCAO_HAB_EXP);
        strcat(msg, "\" para finalizar a insercao de experiencias): ");
        write (sock, msg, sizeof(msg));
        bzero(msg, sizeof(msg));
        
        msgSize = recv(sock , msg , 2000 , 0);
        msg[msgSize] = '\0'; // Finalizando string recebida
        if (!strcmp(msg, FIM_INSERCAO_HAB_EXP))
            break;
        expLen += strlen(msg);
        if (expLen >= MAX_EXPERIENCIAS) {
            strcat(msg, "\nNumero maximo de habilidades excedido");
            break;
        }
        if (expLen == msgSize)
            strcat(novoPerfil.experiencias, msg);
        else {
            novoPerfil.qtdExp++;
            strcat(novoPerfil.experiencias, "(");
            sprintf(strQtdExp, "%d", novoPerfil.qtdExp);
            strcat(novoPerfil.experiencias, strQtdExp);
            strcat(novoPerfil.experiencias, ") ");
            strcat(novoPerfil.experiencias, msg);
        }
        expLen += 4;
        bzero(msg, sizeof(msg));
    }
    // Envia flag dizendo que acabou o cadastro
    bzero(msg, sizeof(msg));
    strcat(msg, FLAG_FINAL);
    write (sock, msg, sizeof(msg));
    // Insercao do perfil novo no arquivo
    pthread_mutex_lock(&lock);

    perfil_file = fopen(FILE_NAME, "a");

    fwrite(&novoPerfil, sizeof(novoPerfil), 1, perfil_file);
    fclose(perfil_file);

    pthread_mutex_unlock(&lock);
}

/**
 * Adiciona uma experiencia a um perfil
 * @param result - string com a mensagem de retorno
 * @param email - email para adicionar a nova experiencia
 * @param exp - experiencia a ser adicionada ao perfil 
 */
void add_experiencia(char* result, char* email, char* exp) {
    FILE *perfil_file ;
    FILE *temp;
    listaPerfil *inicio = NULL, *perfilAtual;
    perfil aux;
    int achou = 0; // 0 - nao achou; 1 - achou
    char strQtdExp[7];

    pthread_mutex_lock(&lock);
    // Le o arquivo ate achar o email pedido
    // Enquanto le o arquivo, guarda os perfis ja lidos para escrever no
    // arquivo novo modificado
    perfil_file = fopen(FILE_NAME, "r");
    while (fread(&aux, sizeof(perfil), 1, perfil_file)) {
        if (strcmp(aux.email, email)) {
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
            aux.qtdExp++;
            strcat(aux.experiencias, "(");
            sprintf(strQtdExp, "%d", aux.qtdExp);
            strcat(aux.experiencias, strQtdExp);
            strcat(aux.experiencias, ") ");
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

    pthread_mutex_unlock(&lock);

    strcat(result, "Perfil removido com sucesso\n");

}


/**
 * Lista perfil/s de acordo com a opcao
 * @param result - string de retorno com a lista de perfis
 * @param filtro - string com a opcao de filtro do cliente 
 * @param opt - LISTAR_CUR      0
 *              LISTAR_HAB      1
 *              LISTAR_ANO      2
 *              LISTAR_ALL      3
 *              LISTAR_EMAIL    4
 */
void list(char* result, char* filtro, int opt) {
    FILE *perfil_file;
    perfil p;
    int ano;
    char *h;

    pthread_mutex_lock(&lock);
    
    // Pode ter que usar sprintf para formatar as strings
    perfil_file = fopen(FILE_NAME, "r");
    switch (opt) {
        case LISTAR_CUR:
            while (fread(&p, sizeof(perfil), 1, perfil_file)) {
                if (!strcmp(p.curso, filtro)) 
                    perfilToString(p, result);
            }
            if (!strcmp(result, ""))
                strcat(result, "Nenhum perfil contem a formacao academica desejada.\n");
            break;
            
        case LISTAR_HAB:
            while (fread(&p, sizeof(perfil), 1, perfil_file)) {
                h = strtok(p.habilidades, ", ");
                while (h != NULL) {
                    if (!strcmp(h, filtro)) {
                        perfilToString(p, result);
                        break;
                    }
                    h = strtok(NULL, ", ");
                }
            }
            if (!strcmp(result, ""))
                strcat(result, "Nenhum perfil contem a habilidade desejada.\n");
            break;

        case LISTAR_ANO:
            ano = atoi(filtro);
            while (fread(&p, sizeof(perfil), 1, perfil_file)) {
                if (ano == p.ano_formatura) 
                    perfilToString(p, result);
            }
            if (!strcmp(result, ""))
                strcat(result, "Nenhum perfil contem o ano de formacao desejado.\n");
            break;

        case LISTAR_ALL:
            while (fread(&p, sizeof(perfil), 1, perfil_file)) {
                perfilToString(p, result);
            }
            if (!strcmp(result, ""))
                strcat(result, "Nenhum perfil cadastrado.\n");
            break;

        case LISTAR_EMAIL:
            while (fread(&p, sizeof(perfil), 1, perfil_file)) {
                if (!strcmp(filtro, p.email)) {
                    perfilToString(p, result);
                    break;
                }
            }
            if (!strcmp(result, ""))
                strcat(result, "Email nao cadastrado.\n");
            break;

        

    }
    fclose(perfil_file);

    pthread_mutex_unlock(&lock);
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
    FILE *perfil_file;
    FILE *temp;
    listaPerfil *inicio = NULL, *perfilAtual;
    perfil aux;
    int achou = 0; // 0 - nao achou; 1 - achou


    pthread_mutex_lock(&lock);
    // Le o arquivo ate achar o email pedido
    // Enquanto le o arquivo, guarda os perfis ja lidos para escrever no
    // arquivo novo modificado
    perfil_file = fopen(FILE_NAME, "r");
    while (fread(&aux, sizeof(perfil), 1, perfil_file)) {
        if (strcmp(aux.email, email)) {
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

    pthread_mutex_unlock(&lock);
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
 * @param sock - cliente
 * @param msg - mensagem vinda do cliente
 * @param result - string de retorno com a resposta do servidor
 */
void comando(int sock, char* msg, char* result) {
    if (!strcmp(CADASTRO_CMD, msg))
        cadastro(sock, result);
    else {
        // Split string para identificar qual listamento de perfis
        // ou remocao de um perfil
        char *tk = strtok(msg, " ");

        if (!strcmp(ADD_EXP_CMD, tk)) {
            tk = strtok(NULL, " ");
            add_experiencia(result, tk, strtok(NULL, ""));
        } else if (!strcmp(LISTAR_CUR_CMD, tk)) {
            tk = strtok(NULL, "");
            list(result, tk, LISTAR_CUR);
        } else if (!strcmp(LISTAR_HAB_CMD, tk)) {
            tk = strtok(NULL, "");
            list(result, tk, LISTAR_HAB);
        } else if (!strcmp(LISTAR_ANO_CMD, tk)) {
            tk = strtok(NULL, "");
            list(result, tk, LISTAR_ANO);
        } else if (!strcmp(LISTAR_ALL_CMD, tk))
            list(result, NULL, LISTAR_ALL);
        else if (!strcmp(LISTAR_EMAIL_CMD, tk)) {
            tk = strtok(NULL, "");
            list(result, tk, LISTAR_EMAIL);
        } else if (!strcmp(REMOVER_CMD, tk)) {
            tk = strtok(NULL, "");
            removePerfil(result, tk);
        } else
            strcat(result, "Comando invalido\n");
        
    }
}

/**
 * Handler que cuida da conexao de um cliente logo apos ele se conectar
 * @param sockfd - cliente
 */
void *recebeCliente(void *sockfd) {
    int client = *(int*)sockfd, msgSize = 0;
    char msg[1000];
    char result [4230 * 50];
    bzero(msg, sizeof(msg));

    while(1) {
        // Recebe uma string
        msgSize = recv(client , msg , 1000 , 0);
		msg[msgSize] = '\0'; // Finalizando string recebida
        if (!strcmp(msg, SAIR_CMD)) {
            close(client);
            break;
        }
        comando(client, msg, result);

        write(client, result, sizeof(result));
		
		//Send the message back to client
        //write(client , msg , strlen(msg));
		
        bzero(msg, sizeof(msg));
        bzero(result, sizeof(result));
    }

}

int main() {
    int serverfd, fd, tam;
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
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex init falhou\n");
        return 1;
    }
    while (1) {
        tam = sizeof(client);
        // Aceita pacote vindo de um cliente 
        fd = accept(serverfd, (struct sockaddr *)&client, &tam);
        
        // Verificacao do que recebeu
        pthread_t thread_id;
        if (fd < 0) {
			printf("Cliente nao foi aceito\n");
		 } else {
			printf("Cliente aceito\n");
            pthread_create(&thread_id, NULL, recebeCliente, (void*) &fd);
        }
    }

    pthread_mutex_destroy(&lock);

    return 0;
}