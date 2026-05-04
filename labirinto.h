/* labirinto.h - Estrutura do labirinto e operacoes principais.
 * O labirinto e representado por duas matrizes: 'mapa' (modificado durante
 * a busca, tesouros/armadilhas sao substituidos por espaco apos coleta) e
 * 'original' (preservado para gerar o arquivo de saida com o caminho marcado).
 */
#ifndef LABIRINTO_H
#define LABIRINTO_H

#include "pilha.h"
#include "mochila.h"

#define MAX 40  /* tamanho maximo do labirinto em cada dimensao */

typedef struct {
    char mapa[MAX][MAX];      /* estado atual, alterado durante a travessia */
    char original[MAX][MAX];  /* copia intocada para salvar solucao final */
    int linhas;
    int colunas;
    Posicao inicio;  /* posicao do 'P' lido do arquivo */
    Posicao saida;   /* posicao do 'S' lido do arquivo */
} Labirinto;

/* Carrega o labirinto do arquivo .txt. Retorna 1 em sucesso, 0 em erro. */
int carregarLabirinto(const char *nomeArquivo, Labirinto *lab);

/* Exibe o labirinto colorido no terminal, destacando a posicao atual. */
void exibirLabirinto(Labirinto *lab, Posicao atual);

/* DFS com backtracking. Preenche 'caminho' e 'tamCaminho' se achar saida. */
int buscarSaida(Labirinto *lab, Mochila *mochila, Posicao caminho[], int *tamCaminho);

/* Grava o labirinto original com '.' marcando o caminho em nomeArquivo. */
int salvarSolucao(const char *nomeArquivo, Labirinto *lab, Posicao caminho[], int tamCaminho);

/* Imprime no terminal o labirinto com o caminho final marcado. */
void exibirCaminhoFinal(Labirinto *lab, Posicao caminho[], int tamCaminho);

#endif