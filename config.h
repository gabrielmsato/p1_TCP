#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORTNO 8080

#define LISTAR_CUR           0
#define LISTAR_CUR_LEN       5

#define LISTAR_HAB           1
#define LISTAR_HAB_LEN      10

#define LISTAR_ANO           2
#define LISTAR_ANO_LEN       3

#define LISTAR_ALL           3
#define LISTAR_ALL_LEN       4

#define LISTAR_EMAIL         4
#define LISTAR_EMAIL_LEN     5

#define MAX_HABILIDADES   2000
#define MAX_EXPERIENCIAS  2000

// não  sei se vou usar
#define MAXALLOC          9000 
#define MAXCAMPOS          200
#define MAX_TAM_STR        150

#define NUMSTR               4

#define sockAddr struct sockaddr 

static const char CADASTRO_CMD[]         = "cadastro";
static const char ADD_EXP_CMD[]          = "experiencia";
static const char LISTAR_CUR_CMD[]       = "curso";
static const char LISTAR_HAB_CMD[]       = "habilidade";
static const char LISTAR_ANO_CMD[]       = "ano";
static const char LISTAR_ALL_CMD[]       = "tudo";
static const char LISTAR_EMAIL_CMD[]     = "email";
static const char REMOVER_CMD[]          = "remover";
static const char SAIR_CMD[]                 = "sair";

static const char FIM_INSERCAO_HAB_EXP[] = "end";
static const char FLAG_FINAL[]           = "fimCadastro";



typedef struct _perfil {
    char email[50];
    char nome[50];
    char sobrenome[50];
    char cidade[30];
    char curso[50];
    int ano_formatura;
    char habilidades[MAX_HABILIDADES];
    int qtdExp;
    char experiencias[MAX_EXPERIENCIAS];
} perfil;

/*
email, nome, sobrenome, cidade de
residência, formação acadêmica, ano de formatura, 
habilidades e experiência profissional.

Exemplo de perfil:
Email: maria_silva@gmail.com
Nome: Maria Sobrenome: Silva
Residência: Campinas
Formação Acadêmica: Ciência da Computação
Ano de Formatura: 2015
Habilidades: Análise de Dados, Internet das Coisas, Computação em Nuvem
Experiência: (1) Estágio de 1 ano na Empresa X, onde trabalhei como analista de dados
(2) Trabalhei com IoT e Computação em Nuvem por 5 anos na Empresa Y
 */