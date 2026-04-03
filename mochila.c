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

    if (m->inicio == NULL || valor < m->inicio->valor) {
        novo->prox = m->inicio;
        m->inicio = novo;
        return 1;
    }

    NoTesouro *atual = m->inicio;
    while (atual->prox != NULL && atual->prox->valor <= valor) {
        atual = atual->prox;
    }

    novo->prox = atual->prox;
    atual->prox = novo;
    return 1;
}

int removerPrimeiroTesouro(Mochila *m, int *valorRemovido) {
    if (m->inicio == NULL) {
        return 0;
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