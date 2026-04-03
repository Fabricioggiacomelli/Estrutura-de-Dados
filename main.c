#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "labirinto.h"
#include "mochila.h"

int main() {
    Labirinto lab;
    Mochila mochila;
    Posicao caminho[MAX * MAX];
    int tamCaminho = 0;

    srand((unsigned int)time(NULL));
    inicializarMochila(&mochila);

    if (!carregarLabirinto("entrada.txt", &lab)) {
        return 1;
    }

    printf("Labirinto carregado com sucesso!\n");
    printf("Pressione Enter para iniciar a busca...");
    getchar();

    if (buscarSaida(&lab, &mochila, caminho, &tamCaminho)) {
        printf("\n\nSaida encontrada!\n");
        printf("Quantidade de passos no caminho final: %d\n", tamCaminho);
        printf("Total de tesouros na mochila: %d\n", calcularTotalMochila(&mochila));
        printf("Mochila final: ");
        imprimirMochila(&mochila);
        printf("\n");

        exibirCaminhoFinal(&lab, caminho, tamCaminho);

        if (salvarSolucao("saida.txt", &lab, caminho, tamCaminho)) {
            printf("\nSolucao salva em saida.txt com sucesso.\n");
        }
    } else {
        printf("\nNenhum caminho ate a saida foi encontrado.\n");
    }

    liberarMochila(&mochila);

    printf("\nPressione Enter para encerrar...");
    getchar();
    return 0;
}