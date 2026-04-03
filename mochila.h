#ifndef MOCHILA_H
#define MOCHILA_H

typedef struct NoTesouro {
    int valor;
    struct NoTesouro *prox;
} NoTesouro;

typedef struct {
    NoTesouro *inicio;
} Mochila;

void inicializarMochila(Mochila *m);
int inserirTesouroOrdenado(Mochila *m, int valor);
int removerPrimeiroTesouro(Mochila *m, int *valorRemovido);
int calcularTotalMochila(Mochila *m);
void imprimirMochila(Mochila *m);
void liberarMochila(Mochila *m);

#endif