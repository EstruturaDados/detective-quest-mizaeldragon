#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 10


typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
    char pista[50]; 
} Sala;

Sala* criarSala(char *nome, char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    if (pista != NULL)
        strcpy(nova->pista, pista);
    else
        strcpy(nova->pista, "");
    nova->esquerda = nova->direita = NULL;
    return nova;
}


typedef struct NoPista {
    char pista[50];
    struct NoPista *esq;
    struct NoPista *dir;
} NoPista;

NoPista* novoNoPista(char *pista) {
    NoPista *novo = (NoPista*) malloc(sizeof(NoPista));
    strcpy(novo->pista, pista);
    novo->esq = novo->dir = NULL;
    return novo;
}

NoPista* inserirPista(NoPista *raiz, char *pista) {
    if (raiz == NULL) return novoNoPista(pista);
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esq = inserirPista(raiz->esq, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->dir = inserirPista(raiz->dir, pista);
    return raiz;
}

void listarPistas(NoPista *raiz) {
    if (raiz != NULL) {
        listarPistas(raiz->esq);
        printf("- %s\n", raiz->pista);
        listarPistas(raiz->dir);
    }
}


typedef struct Suspeito {
    char pista[50];
    char nome[50];
    struct Suspeito *prox;
} Suspeito;

Suspeito* tabelaHash[TAM_HASH];

int hashFunc(char *pista) {
    int soma = 0;
    for (int i = 0; pista[i] != '\0'; i++)
        soma += pista[i];
    return soma % TAM_HASH;
}

void inserirHash(char *pista, char *nome) {
    int indice = hashFunc(pista);
    Suspeito *novo = (Suspeito*) malloc(sizeof(Suspeito));
    strcpy(novo->pista, pista);
    strcpy(novo->nome, nome);
    novo->prox = tabelaHash[indice];
    tabelaHash[indice] = novo;
}

void listarHash() {
    printf("\n--- Relação Pistas → Suspeitos ---\n");
    for (int i = 0; i < TAM_HASH; i++) {
        Suspeito *atual = tabelaHash[i];
        while (atual != NULL) {
            printf("Pista: %-25s | Suspeito: %s\n", atual->pista, atual->nome);
            atual = atual->prox;
        }
    }
}

void suspeitoMaisCitado() {
    char nomes[50][50];
    int contagens[50];
    int total = 0;

    for (int i = 0; i < TAM_HASH; i++) {
        Suspeito *atual = tabelaHash[i];
        while (atual != NULL) {
            int encontrado = 0;
            for (int j = 0; j < total; j++) {
                if (strcmp(nomes[j], atual->nome) == 0) {
                    contagens[j]++;
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) {
                strcpy(nomes[total], atual->nome);
                contagens[total] = 1;
                total++;
            }
            atual = atual->prox;
        }
    }

    int max = 0;
    char culpado[50];
    for (int i = 0; i < total; i++) {
        if (contagens[i] > max) {
            max = contagens[i];
            strcpy(culpado, nomes[i]);
        }
    }

    if (total > 0)
        printf("\n🕵️ O principal suspeito é: %s (citado %d vezes)\n", culpado, max);
    else
        printf("\nNenhum suspeito foi registrado ainda.\n");
}


void explorarSalas(Sala *raiz, NoPista **arvorePistas) {
    Sala *atual = raiz;
    char opcao;
    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf("💡 Você encontrou uma pista: %s\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
            if (strcmp(atual->pista, "Luvas rasgadas") == 0)
                inserirHash(atual->pista, "Jardineiro");
            else if (strcmp(atual->pista, "Perfume caro") == 0)
                inserirHash(atual->pista, "Governanta");
            else if (strcmp(atual->pista, "Relógio quebrado") == 0)
                inserirHash(atual->pista, "Motorista");
        }

        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("🚪 Fim do caminho nesta direção.\n");
            break;
        }

        printf("Escolha (e = esquerda, d = direita, s = sair): ");
        scanf(" %c", &opcao);

        if (opcao == 'e') atual = atual->esquerda;
        else if (opcao == 'd') atual = atual->direita;
        else if (opcao == 's') break;
        else printf("Opção inválida!\n");
    }
}



int main() {
    Sala *hall = criarSala("Hall de Entrada", "");
    hall->esquerda = criarSala("Biblioteca", "Luvas rasgadas");
    hall->direita = criarSala("Sala de Jantar", "Perfume caro");
    hall->esquerda->esquerda = criarSala("Porão", "Relógio quebrado");
    hall->esquerda->direita = criarSala("Jardim", "");
    hall->direita->direita = criarSala("Cozinha", "");

    NoPista *arvorePistas = NULL;
    for (int i = 0; i < TAM_HASH; i++) tabelaHash[i] = NULL;

    printf("🔎 BEM-VINDO AO DESAFIO DETECTIVE QUEST 🔎\n");

    explorarSalas(hall, &arvorePistas);

    printf("\n📜 Pistas encontradas:\n");
    listarPistas(arvorePistas);

    listarHash();
    suspeitoMaisCitado();

    printf("\nFim da investigação.\n");
    return 0;
}
