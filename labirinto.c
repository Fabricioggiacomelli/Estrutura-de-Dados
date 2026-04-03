#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "labirinto.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

static void esperar(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

static void limparTela() {
    printf("\033[2J\033[H");
}

static int dentroLimites(Labirinto *lab, int lin, int col) {
    return lin >= 0 && lin < lab->linhas && col >= 0 && col < lab->colunas;
}

static int ehMovimentoValido(Labirinto *lab, int visitado[MAX][MAX], int lin, int col) {
    if (!dentroLimites(lab, lin, col)) {
        return 0;
    }

    if (visitado[lin][col]) {
        return 0;
    }

    if (lab->mapa[lin][col] == '#') {
        return 0;
    }

    return 1;
}

int carregarLabirinto(const char *nomeArquivo, Labirinto *lab) {
    FILE *arq = fopen(nomeArquivo, "r");
    if (arq == NULL) {
        printf("Erro: nao foi possivel abrir o arquivo %s\n", nomeArquivo);
        return 0;
    }

    if (fscanf(arq, "%d %d", &lab->linhas, &lab->colunas) != 2) {
        printf("Erro: nao foi possivel ler as dimensoes do labirinto.\n");
        fclose(arq);
        return 0;
    }

    if (lab->linhas <= 0 || lab->linhas > MAX || lab->colunas <= 0 || lab->colunas > MAX) {
        printf("Erro: dimensoes invalidas. O maximo permitido eh 40x40.\n");
        fclose(arq);
        return 0;
    }

    fgetc(arq);

    int encontrouP = 0;
    int encontrouS = 0;
    char linha[256];

    for (int i = 0; i < lab->linhas; i++) {
        if (fgets(linha, sizeof(linha), arq) == NULL) {
            printf("Erro: arquivo com menos linhas do que o informado.\n");
            fclose(arq);
            return 0;
        }

        int tamanho = (int)strlen(linha);

        while (tamanho > 0 && (linha[tamanho - 1] == '\n' || linha[tamanho - 1] == '\r')) {
            linha[tamanho - 1] = '\0';
            tamanho--;
        }

        if (tamanho < lab->colunas) {
            printf("Erro: a linha %d do labirinto possui menos caracteres do que o esperado.\n", i + 1);
            fclose(arq);
            return 0;
        }

        for (int j = 0; j < lab->colunas; j++) {
            lab->mapa[i][j] = linha[j];
            lab->original[i][j] = linha[j];

            if (linha[j] == 'P') {
                lab->inicio.linha = i;
                lab->inicio.coluna = j;
                encontrouP = 1;
            }

            if (linha[j] == 'S') {
                lab->saida.linha = i;
                lab->saida.coluna = j;
                encontrouS = 1;
            }
        }
    }

    fclose(arq);

    if (!encontrouP) {
        printf("Erro: personagem inicial 'P' nao encontrado.\n");
        return 0;
    }

    if (!encontrouS) {
        printf("Erro: saida 'S' nao encontrada.\n");
        return 0;
    }

    return 1;
}

void exibirLabirinto(Labirinto *lab, Posicao atual) {
    limparTela();

    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++) {
            if (i == atual.linha && j == atual.coluna) {
                printf("@");
            } else {
                printf("%c", lab->mapa[i][j]);
            }
        }
        printf("\n");
    }
}

int buscarSaida(Labirinto *lab, Mochila *mochila, Posicao caminho[], int *tamCaminho) {
    Pilha pilha;
    inicializarPilha(&pilha);

    int visitado[MAX][MAX] = {0};
    Posicao pai[MAX][MAX];
    int encontrou = 0;

    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            pai[i][j].linha = -1;
            pai[i][j].coluna = -1;
        }
    }

    empilhar(&pilha, lab->inicio);
    visitado[lab->inicio.linha][lab->inicio.coluna] = 1;

    while (!pilhaVazia(&pilha)) {
        Posicao atual;
        topoPilha(&pilha, &atual);

        exibirLabirinto(lab, atual);
        printf("\nMochila: ");
        imprimirMochila(mochila);
        printf("\nTotal: %d\n", calcularTotalMochila(mochila));
        esperar(250);

        if (atual.linha == lab->saida.linha && atual.coluna == lab->saida.coluna) {
            encontrou = 1;
            break;
        }

        int movimentos[4][2] = {
            {-1, 0},
            {0, 1},
            {1, 0},
            {0, -1}
        };

        int moveu = 0;

        for (int k = 0; k < 4; k++) {
            int nl = atual.linha + movimentos[k][0];
            int nc = atual.coluna + movimentos[k][1];

            if (ehMovimentoValido(lab, visitado, nl, nc)) {
                visitado[nl][nc] = 1;

                pai[nl][nc] = atual;

                char celula = lab->mapa[nl][nc];

                if (celula == 'T') {
                    int valor = rand() % 100 + 1;
                    if (inserirTesouroOrdenado(mochila, valor)) {
                        printf("\nTesouro encontrado em (%d,%d)! Valor: %d\n", nl, nc, valor);
                    } else {
                        printf("\nErro de memoria ao guardar tesouro.\n");
                    }
                    lab->mapa[nl][nc] = ' ';
                } else if (celula == 'A') {
                    int removido;
                    if (removerPrimeiroTesouro(mochila, &removido)) {
                        printf("\nArmadilha em (%d,%d)! Tesouro perdido: %d\n", nl, nc, removido);
                    } else {
                        printf("\nArmadilha em (%d,%d)! Mas a mochila estava vazia.\n", nl, nc);
                    }
                    lab->mapa[nl][nc] = ' ';
                }

                Posicao prox;
                prox.linha = nl;
                prox.coluna = nc;
                empilhar(&pilha, prox);
                moveu = 1;
                break;
            }
        }

        if (!moveu) {
            Posicao removida;
            desempilhar(&pilha, &removida);
        }
    }

    if (!encontrou) {
        liberarPilha(&pilha);
        *tamCaminho = 0;
        return 0;
    }

    Posicao atual = lab->saida;
    Posicao inverso[MAX * MAX];
    int tam = 0;

    while (!(atual.linha == -1 && atual.coluna == -1)) {
        inverso[tam++] = atual;

        if (atual.linha == lab->inicio.linha && atual.coluna == lab->inicio.coluna) {
            break;
        }

        atual = pai[atual.linha][atual.coluna];
    }

    *tamCaminho = 0;
    for (int i = tam - 1; i >= 0; i--) {
        caminho[*tamCaminho] = inverso[i];
        (*tamCaminho)++;
    }

    liberarPilha(&pilha);
    return 1;
}

int salvarSolucao(const char *nomeArquivo, Labirinto *lab, Posicao caminho[], int tamCaminho) {
    FILE *arq = fopen(nomeArquivo, "w");
    if (arq == NULL) {
        printf("Erro: nao foi possivel criar o arquivo %s\n", nomeArquivo);
        return 0;
    }

    char copia[MAX][MAX];

    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++) {
            copia[i][j] = lab->original[i][j];
        }
    }

    for (int i = 0; i < tamCaminho; i++) {
        int l = caminho[i].linha;
        int c = caminho[i].coluna;

        if (copia[l][c] != 'P' && copia[l][c] != 'S') {
            copia[l][c] = '.';
        }
    }

    fprintf(arq, "%d %d\n", lab->linhas, lab->colunas);
    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++) {
            fputc(copia[i][j], arq);
        }
        fputc('\n', arq);
    }

    fclose(arq);
    return 1;
}