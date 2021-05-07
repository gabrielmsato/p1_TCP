#include <string.h>
#include <stdio.h>
#include <strings.h>

typedef struct _perfil {
    char email[50];
    char nome[50];
    int ano_formatura;
    char habilidades[2000];
    // int qtdHab;
    char experiencias[2000];
    // int qtdExp;
} perfil;

int main() {
    FILE *f = fopen("a.txt", "w");
    perfil p;
    bzero(p.email, sizeof(p.email));
    strcat(p.email, "email");
    bzero(p.nome, sizeof(p.nome));
    strcat(p.nome, "nome");
    p.ano_formatura = 2050;
    bzero(p.habilidades, sizeof(p.habilidades));
    strcat(p.habilidades, "nenhuma");
    bzero(p.experiencias, sizeof(p.experiencias));
    strcat(p.experiencias, "experiencias");

    fwrite(&p, sizeof(perfil), 1, f);

    bzero(&p, sizeof(p));

    strcat(p.email, "sera que deu certo?");
    strcat(p.nome, "AAAAAA");
    p.ano_formatura = 9999;
    strcat(p.habilidades, "hm");
    strcat(p.experiencias, "BBBBBBBB");

    fwrite(&p, sizeof(perfil), 1, f);

    fclose(f);
    f = fopen("a.txt", "r");

    while(fread(&p, sizeof(perfil), 1, f)) {
        printf("%s\n%s\n%d\n%s\n%s\n", p.email, p.nome, p.ano_formatura, p.habilidades,p.experiencias);
    }

    char g[50] = "Eu estou testando\n";
    char *tk = strtok(g, " ");
    printf ("%s\n", tk);
    tk = strtok(NULL, "");
    printf ("%s", tk);

    return 0;
}