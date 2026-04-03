#ifndef LABIRINTO_H
#define LABIRINTO_H

#include "pilha.h"
#include "mochila.h"

#define MAX 40

typedef struct {
    char mapa[MAX][MAX];
    char original[MAX][MAX];
    int linhas;
    int colunas;
    Posicao inicio;
    Posicao saida;
} Labirinto;

int carregarLabirinto(const char *nomeArquivo, Labirinto *lab);
void exibirLabirinto(Labirinto *lab, Posicao atual);
int buscarSaida(Labirinto *lab, Mochila *mochila, Posicao caminho[], int *tamCaminho);
int salvarSolucao(const char *nomeArquivo, Labirinto *lab, Posicao caminho[], int tamCaminho);

#endif