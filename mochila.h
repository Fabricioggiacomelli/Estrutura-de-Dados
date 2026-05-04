/* mochila.h - Lista encadeada ordenada de tesouros.
 * A lista e mantida em ordem crescente de valor para que o tesouro de menor
 * valor esteja sempre na primeira posicao — assim, ao cair numa armadilha,
 * a perda e minimizada (estrategia de maximizacao do PDF).
 */
#ifndef MOCHILA_H
#define MOCHILA_H

/* No da lista: armazena o valor do tesouro coletado. */
typedef struct NoTesouro {
    int valor;
    struct NoTesouro *prox;
} NoTesouro;

/* Mochila e a cabeca da lista encadeada ordenada. */
typedef struct {
    NoTesouro *inicio;
} Mochila;

void inicializarMochila(Mochila *m);
int inserirTesouroOrdenado(Mochila *m, int valor); /* insercao em O(n), mantendo ordem */
int removerPrimeiroTesouro(Mochila *m, int *valorRemovido); /* remove o de menor valor */
int calcularTotalMochila(Mochila *m);
void imprimirMochila(Mochila *m);
void liberarMochila(Mochila *m);

#endif