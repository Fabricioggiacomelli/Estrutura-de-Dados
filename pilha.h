#ifndef PILHA_H
#define PILHA_H

typedef struct {
    int linha;
    int coluna;
} Posicao;

typedef struct NoPilha {
    Posicao pos;
    struct NoPilha *prox;
} NoPilha;

typedef struct {
    NoPilha *topo;
} Pilha;

void inicializarPilha(Pilha *p);
int pilhaVazia(Pilha *p);
int empilhar(Pilha *p, Posicao pos);
int desempilhar(Pilha *p, Posicao *pos);
int topoPilha(Pilha *p, Posicao *pos);
void liberarPilha(Pilha *p);

#endif