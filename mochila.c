/* mochila.c - Lista encadeada ordenada de tesouros.
 * Invariante: a lista esta sempre em ordem crescente de valor,
 * garantindo que m->inicio aponte para o tesouro de menor valor.
 * Isso minimiza a perda ao cair numa armadilha (remove o primeiro). */
#include <stdio.h>
#include <stdlib.h>
#include "mochila.h"

void inicializarMochila(Mochila *m) {
    m->inicio = NULL;
}

int inserirTesouroOrdenado(Mochila *m, int valor) {
    NoTesouro *novo = (NoTesouro *)malloc(sizeof(NoTesouro));
    if (novo == NULL) {
        return 0;
    }

    novo->valor = valor;
    novo->prox = NULL;

    /* insercao no inicio se a lista esta vazia ou o novo valor e o menor */
    if (m->inicio == NULL || valor < m->inicio->valor) {
        novo->prox = m->inicio;
        m->inicio = novo;
        return 1;
    }

    /* percorre ate encontrar o ponto de insercao (manter ordem crescente) */
    NoTesouro *atual = m->inicio;
    while (atual->prox != NULL && atual->prox->valor <= valor) {
        atual = atual->prox;
    }

    novo->prox = atual->prox;
    atual->prox = novo;
    return 1;
}

/* Remove o no do inicio (menor valor) — acionado pela armadilha 'A'. */
int removerPrimeiroTesouro(Mochila *m, int *valorRemovido) {
    if (m->inicio == NULL) {
        return 0;  /* mochila ja estava vazia */
    }

    NoTesouro *temp = m->inicio;
    *valorRemovido = temp->valor;
    m->inicio = temp->prox;
    free(temp);
    return 1;
}

int calcularTotalMochila(Mochila *m) {
    int total = 0;
    NoTesouro *atual = m->inicio;

    while (atual != NULL) {
        total += atual->valor;
        atual = atual->prox;
    }

    return total;
}

void imprimirMochila(Mochila *m) {
    NoTesouro *atual = m->inicio;

    if (atual == NULL) {
        printf("[vazia]");
        return;
    }

    while (atual != NULL) {
        printf("%d ", atual->valor);
        atual = atual->prox;
    }
}

void liberarMochila(Mochila *m) {
    NoTesouro *atual = m->inicio;
    while (atual != NULL) {
        NoTesouro *temp = atual;
        atual = atual->prox;
        free(temp);
    }
    m->inicio = NULL;
}