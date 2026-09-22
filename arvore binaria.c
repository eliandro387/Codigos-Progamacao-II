#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

typedef struct No {
    int valor;
    struct No *esquerda;
    struct No *direita;
} No;

// Protótipos das funções
No* criarNo(int valor);
No* inserir(No* raiz, int valor);
No* obterMenorNo(No* raiz);
No* remover(No* raiz, int valor);
No* buscar(No* raiz, int valor);
void preOrdem(No* raiz);
void emOrdem(No* raiz);
void posOrdem(No* raiz);
void liberarArvore(No* raiz);
void menu();

// Função para alocar um novo nó
No* criarNo(int valor) {
    No* novoNo = (No*)malloc(sizeof(No));
    if (novoNo == NULL) {
        printf("Erro de alocação de memoria!\n");
        exit(1);
    }
    novoNo->valor = valor;
    novoNo->esquerda = NULL;
    novoNo->direita = NULL;
    return novoNo;
}

// Função para inserir um elemento na BST
No* inserir(No* raiz, int valor) {
    if (raiz == NULL) {
        return criarNo(valor);
    }
    
    if (valor < raiz->valor) {
        raiz->esquerda = inserir(raiz->esquerda, valor);
    } else if (valor > raiz->valor) {
        raiz->direita = inserir(raiz->direita, valor);
    } else {
        printf("Valor %d ja existe na arvore. Ignorado.\n", valor);
    }
    return raiz;
}

// Função auxiliar para encontrar o menor nó da subárvore (usado na remoção)
No* obterMenorNo(No* raiz) {
    No* atual = raiz;
    while (atual && atual->esquerda != NULL) {
        atual = atual->esquerda;
    }
    return atual;
}

// Função para remover um elemento da BST 
No* remover(No* raiz, int valor) {
    if (raiz == NULL) {
        return NULL;
    }

    if (valor < raiz->valor) {
        raiz->esquerda = remover(raiz->esquerda, valor);
    } else if (valor > raiz->valor) {
        raiz->direita = remover(raiz->direita, valor);
    } else {
        // Encontrou o nó a ser removido

        // Caso 1 e 2: Sem filho à esquerda ou sem filho à direita
        if (raiz->esquerda == NULL) {
            No* temp = raiz->direita;
            free(raiz);
            return temp;
        } else if (raiz->direita == NULL) {
            No* temp = raiz->esquerda;
            free(raiz);
            return temp;
        }

        // Caso 3: Nó com 2 filhos
        No* temp = obterMenorNo(raiz->direita);
        raiz->valor = temp->valor;
        raiz->direita = remover(raiz->direita, temp->valor);
    }
    return raiz;
}

// Função de busca
No* buscar(No* raiz, int valor) {
    if (raiz == NULL || raiz->valor == valor) 
        return raiz;
    if (valor > raiz->valor)
        return buscar(raiz->direita, valor);
    return buscar(raiz->esquerda, valor);
}

// IMPLEMENTAÇÃO DOS PERCURSOS

// Percurso Pré-ordem (Raiz -> Esquerda -> Direita)
void preOrdem(No* raiz) {
    if (raiz != NULL) {
        printf("%d ", raiz->valor);
        preOrdem(raiz->esquerda);
        preOrdem(raiz->direita);
    }
}

// Percurso Em Ordem (Esquerda -> Raiz -> Direita)
void emOrdem(No* raiz) {
    if (raiz != NULL) {
        emOrdem(raiz->esquerda);
        printf("%d ", raiz->valor);
        emOrdem(raiz->direita);
    }
}

// Percurso Pós-ordem (Esquerda -> Direita -> Raiz)
void posOrdem(No* raiz) {
    if (raiz != NULL) {
        posOrdem(raiz->esquerda);
        posOrdem(raiz->direita);
        printf("%d ", raiz->valor);
    }
}

// LIBERAÇÃO DE MEMÓRIA (Pós-ordem)
void liberarArvore(No* raiz) {
    if (raiz != NULL) {
        liberarArvore(raiz->esquerda);
        liberarArvore(raiz->direita);
        free(raiz);
    }
}

// MENU INTERATIVO
void menu() {
    No* raiz = NULL;
    int opcao, valor, opcao1;

    do {
        printf("\nMenu:\n1. Inserir valor\n2. Buscar valor\n3. Remover valor\n4. Percorrer arvore\n0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 0:
                printf("Liberando memoria e saindo...\n");
                liberarArvore(raiz);
                raiz = NULL;
                break;

            case 1:
                printf("Digite o valor a ser inserido: ");
                scanf("%d", &valor);
                raiz = inserir(raiz, valor);
                break;

            case 2:
                printf("Digite um valor a ser buscado: ");
                scanf("%d", &valor);
                if (buscar(raiz, valor) != NULL)
                    printf("Valor %d encontrado na arvore.\n", valor);
                else
                    printf("Valor %d não encontrado.\n", valor);
                break;

            case 3:
                printf("Digite o valor a ser removido: ");
                scanf("%d", &valor);
                if (buscar(raiz, valor) != NULL) {
                    raiz = remover(raiz, valor);
                    printf("Valor %d removido da arvore.\n", valor);
                } else {
                    printf("Valor %d não encontrado.\n", valor);
                }
                break;

            case 4:
                if (raiz == NULL) {
                    printf("Arvore vazia!\n");
                    break;
                }
                printf("\n1. Pre-ordem\n2. Em ordem\n3. Pos-ordem\n");
                printf("Escolha uma opção: "); 
                scanf("%d", &opcao1);

                switch (opcao1) {
                    case 1:
                        printf("Pre-ordem: ");
                        preOrdem(raiz);
                        printf("\n");
                        break;
                    case 2:
                        printf("Em ordem: ");
                        emOrdem(raiz);
                        printf("\n");
                        break;
                    case 3:
                        printf("Pos-ordem: ");
                        posOrdem(raiz);
                        printf("\n");
                        break;
                    default:
                        printf("Opção de percurso invalida!\n");
                        break;
                }
                break;

            default:
                printf("Opção invalida!\n");
        }
    } while(opcao != 0);
}

int main() {
    setlocale(LC_ALL, "pt_BR.UTF-8");
    menu();
    return 0;
}
