#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"

void inicializarPilha(Pilha *p) {
    p->topo = NULL;
}

int pilhaVazia(Pilha *p) {
    return p->topo == NULL;
}

int empilhar(Pilha *p, Posicao pos) {
    NoPilha *novo = (NoPilha *)malloc(sizeof(NoPilha));
    if (novo == NULL) {
        return 0;
    }

    novo->pos = pos;
    novo->prox = p->topo;
    p->topo = novo;
    return 1;
}

int desempilhar(Pilha *p, Posicao *pos) {
    if (pilhaVazia(p)) {
        return 0;
    }

    NoPilha *temp = p->topo;
    *pos = temp->pos;
    p->topo = temp->prox;
    free(temp);
    return 1;
}

int topoPilha(Pilha *p, Posicao *pos) {
    if (pilhaVazia(p)) {
        return 0;
    }

    *pos = p->topo->pos;
    return 1;
}

void liberarPilha(Pilha *p) {
    Posicao lixo;
    while (desempilhar(p, &lixo)) {
    }
}