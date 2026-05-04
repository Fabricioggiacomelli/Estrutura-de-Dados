/* labirinto.c - Carregamento, exibicao e busca (DFS + backtracking) do labirinto.
 *
 * Algoritmo de busca (buscarSaida):
 *   DFS iterativo com pilha explicita. A cada iteracao, o topo e inspecionado
 *   (peek) sem remover. Se ha vizinho valido, ele e empilhado (avanco). Se nao
 *   ha, o topo e desempilhado (backtracking). O array 'pai' registra, para cada
 *   celula, qual celula a originou — usado ao final para reconstruir o caminho.
 */
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

#define RESET   "\033[0m"
#define VERMELHO "\033[31m"
#define VERDE   "\033[32m"
#define AMARELO "\033[33m"
#define AZUL    "\033[34m"
#define CIANO   "\033[36m"
#define BRANCO  "\033[37m"

/* Pausa multiplataforma usada para animar a travessia no terminal. */
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

static void imprimirCelulaColorida(char c) {
    switch (c) {
        case '#':
            printf(BRANCO "%c" RESET, c);
            break;
        case 'T':
            printf(AMARELO "%c" RESET, c);
            break;
        case 'A':
            printf(VERMELHO "%c" RESET, c);
            break;
        case 'S':
            printf(VERDE "%c" RESET, c);
            break;
        case 'P':
            printf(AZUL "%c" RESET, c);
            break;
        case '.':
            printf(CIANO "%c" RESET, c);
            break;
        default:
            printf("%c", c);
            break;
    }
}

/* Le o arquivo .txt: primeira linha contem "linhas colunas", as seguintes
 * contem o mapa caractere a caractere. Localiza 'P' e 'S' durante a leitura.
 * Armazena o mapa tanto em lab->mapa (usado na busca) quanto em lab->original
 * (usado para gerar o arquivo de saida sem alteracoes da travessia). */
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

    printf(CIANO "=========== LABIRINTO DE DADOS ===========\n" RESET);
    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++) {
            if (i == atual.linha && j == atual.coluna) {
                printf(AZUL "@" RESET);
            } else {
                imprimirCelulaColorida(lab->mapa[i][j]);
            }
        }
        printf("\n");
    }
}

/* DFS iterativo com backtracking para encontrar a saida do labirinto.
 *
 * Estruturas principais:
 *   pilha    - caminho atual em exploracao (peek = posicao corrente)
 *   visitado - celulas ja colocadas na pilha (nao sao revisitadas)
 *   pai      - para cada celula, qual celula a adicionou a pilha;
 *              permite reconstruir o caminho de P ate S ao final
 *
 * Fluxo: peek no topo -> tenta mover para vizinho valido -> empilha.
 *        Se nenhum vizinho valido, desempilha (backtracking).
 *        Ao chegar em 'S', reconstroi o caminho via array pai (S->P)
 *        e inverte para retornar na ordem correta (P->S). */
int buscarSaida(Labirinto *lab, Mochila *mochila, Posicao caminho[], int *tamCaminho) {
    Pilha pilha;
    inicializarPilha(&pilha);

    int visitado[MAX][MAX] = {0};
    Posicao pai[MAX][MAX];
    int encontrou = 0;
    int passo = 0;

    /* inicializa pai com (-1,-1) para identificar celulas sem predecessor */
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
        passo++;

        exibirLabirinto(lab, atual);
        printf("\n" CIANO "--------------- STATUS ---------------\n" RESET);
        printf("Passo atual: %d\n", passo);
        printf("Posicao atual: (%d, %d)\n", atual.linha, atual.coluna);
        printf("Mochila: ");
        imprimirMochila(mochila);
        printf("\nTotal da mochila: %d\n", calcularTotalMochila(mochila));
        printf(CIANO "--------------------------------------\n" RESET);

        esperar(220);

        if (atual.linha == lab->saida.linha && atual.coluna == lab->saida.coluna) {
            encontrou = 1;
            break;
        }

        /* ordem de preferencia: cima, direita, baixo, esquerda */
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
                    /* valor aleatorio 1-100 sorteado no momento da coleta */
                    int valor = rand() % 100 + 1;
                    if (inserirTesouroOrdenado(mochila, valor)) {
                        exibirLabirinto(lab, atual);
                        printf("\n" CIANO "--------------- EVENTO ----------------\n" RESET);
                        printf(AMARELO "Tesouro encontrado!\n" RESET);
                        printf("Posicao: (%d, %d)\n", nl, nc);
                        printf("Valor sorteado: %d\n", valor);
                        printf("Mochila agora: ");
                        imprimirMochila(mochila);
                        printf("\n" CIANO "---------------------------------------\n" RESET);
                        esperar(700);
                    } else {
                        printf("\nErro de memoria ao guardar tesouro.\n");
                        esperar(700);
                    }
                    lab->mapa[nl][nc] = ' ';
                } else if (celula == 'A') {
                    int removido;
                    exibirLabirinto(lab, atual);
                    printf("\n" CIANO "--------------- EVENTO ----------------\n" RESET);
                    if (removerPrimeiroTesouro(mochila, &removido)) {
                        printf(VERMELHO "Armadilha acionada!\n" RESET);
                        printf("Posicao: (%d, %d)\n", nl, nc);
                        printf("Tesouro perdido: %d\n", removido);
                    } else {
                        printf(VERMELHO "Armadilha acionada!\n" RESET);
                        printf("Posicao: (%d, %d)\n", nl, nc);
                        printf("A mochila estava vazia.\n");
                    }
                    printf("Mochila agora: ");
                    imprimirMochila(mochila);
                    printf("\n" CIANO "---------------------------------------\n" RESET);
                    esperar(700);
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

            if (!pilhaVazia(&pilha)) {
                exibirLabirinto(lab, removida);
                printf("\n" CIANO "--------------- EVENTO ----------------\n" RESET);
                printf("Beco sem saida em (%d, %d).\n", removida.linha, removida.coluna);
                printf("Realizando backtracking...\n");
                printf(CIANO "---------------------------------------\n" RESET);
                esperar(450);
            }
        }
    }

    if (!encontrou) {
        liberarPilha(&pilha);
        *tamCaminho = 0;
        return 0;
    }

    /* Reconstrucao do caminho: segue o array pai de S ate P em ordem inversa,
     * depois inverte para retornar o caminho na ordem P->S. */
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

void exibirCaminhoFinal(Labirinto *lab, Posicao caminho[], int tamCaminho) {
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

    printf("\n" CIANO "=========== CAMINHO FINAL ENCONTRADO ===========\n" RESET);
    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++) {
            imprimirCelulaColorida(copia[i][j]);
        }
        printf("\n");
    }

    printf("\nSequencia de posicoes do caminho:\n");
    for (int i = 0; i < tamCaminho; i++) {
        printf("(%d,%d)", caminho[i].linha, caminho[i].coluna);
        if (i < tamCaminho - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
}