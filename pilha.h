/* pilha.h - Pilha dinamica de posicoes usada pelo algoritmo de backtracking.
 * Implementada como lista encadeada para suportar labirintos de ate 40x40
 * sem limite de tamanho fixo em tempo de compilacao.
 */
#ifndef PILHA_H
#define PILHA_H

/* Coordenada (linha, coluna) de uma celula no labirinto. */
typedef struct {
    int linha;
    int coluna;
} Posicao;

/* No interno da pilha — armazena uma posicao e aponta para o proximo no. */
typedef struct NoPilha {
    Posicao pos;
    struct NoPilha *prox;
} NoPilha;

/* Pilha encadeada: acesso O(1) em push/pop/peek. */
typedef struct {
    NoPilha *topo;
} Pilha;

void inicializarPilha(Pilha *p);
int pilhaVazia(Pilha *p);
int empilhar(Pilha *p, Posicao pos);    /* retorna 0 se malloc falhar */
int desempilhar(Pilha *p, Posicao *pos);
int topoPilha(Pilha *p, Posicao *pos);  /* peek sem remover */
void liberarPilha(Pilha *p);

#endif