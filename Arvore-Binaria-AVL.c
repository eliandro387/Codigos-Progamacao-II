#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

// Estrutura do Nó da Árvore AVL
typedef struct No {
    int valor;
    int altura;
    struct No *esquerda;
    struct No *direita;
} No;

// Protótipos das funções
int obterAltura(No* n);
int maior(int a, int b);
int obterFatorBalanceamento(No* n);
No* criarNo(int valor);

// Funções de Rotação (Simples e Duplas)
No* rotacaoDireita(No* y);
No* rotacaoEsquerda(No* x);
No* rotacaoEsquerdaDireita(No* no);
No* rotacaoDireitaEsquerda(No* no);

No* rebalancear(No* no);
No* inserir(No* raiz, int valor);
No* obterMenorNo(No* raiz);
No* remover(No* raiz, int valor);
No* buscar(No* raiz, int valor);
void preOrdem(No* raiz);
void emOrdem(No* raiz);
void posOrdem(No* raiz);
void exibirEstadoAVL(No* raiz);
void liberarArvore(No* raiz);
void menu();

// Função para obter a altura
int obterAltura(No* n) {
    if (n == NULL) return 0;
    return n->altura;
}

// Retorna o maior valor entre dois inteiros
int maior(int a, int b) {
    return (a > b) ? a : b;
}

// Fator de balanceamento: Altura(Esq) - Altura(Dir)
int obterFatorBalanceamento(No* n) {
    if (n == NULL) return 0;
    return obterAltura(n->esquerda) - obterAltura(n->direita);
}

// Função para alocar dinamicamente um novo nó
No* criarNo(int valor) {
    No* novoNo = (No*)malloc(sizeof(No));
    if (novoNo == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    novoNo->valor = valor;
    novoNo->altura = 1; // Todo nó folha começa com altura 1
    novoNo->esquerda = NULL;
    novoNo->direita = NULL;
    return novoNo;
}

// 1. Rotação Simples à Direita
No* rotacaoDireita(No* y) {
    No* x = y->esquerda;
    No* T2 = x->direita;

    x->direita = y;
    y->esquerda = T2;

    y->altura = 1 + maior(obterAltura(y->esquerda), obterAltura(y->direita));
    x->altura = 1 + maior(obterAltura(x->esquerda), obterAltura(x->direita));

    return x;
}

// 2. Rotação Simples à Esquerda
No* rotacaoEsquerda(No* x) {
    No* y = x->direita;
    No* T2 = y->esquerda;

    y->esquerda = x;
    x->direita = T2;

    x->altura = 1 + maior(obterAltura(x->esquerda), obterAltura(x->direita));
    y->altura = 1 + maior(obterAltura(y->esquerda), obterAltura(y->direita));

    return y;
}

// 3. Rotação Dupla Esquerda-Direita
No* rotacaoEsquerdaDireita(No* no) {
    no->esquerda = rotacaoEsquerda(no->esquerda);
    return rotacaoDireita(no);
}

// 4. Rotação Dupla Direita-Esquerda
No* rotacaoDireitaEsquerda(No* no) {
    no->direita = rotacaoDireita(no->direita);
    return rotacaoEsquerda(no);
}

// Rebalanceamento da Árvore AVL
No* rebalancear(No* no) {
    if (no == NULL) return NULL;

    // 1. Atualizar a altura
    no->altura = 1 + maior(obterAltura(no->esquerda), obterAltura(no->direita));

    // 2. Verificar Fator de Balanceamento
    int fb = obterFatorBalanceamento(no);

    // Caso Pensa para a Esquerda (FB > 1)
    if (fb > 1) {
        if (obterFatorBalanceamento(no->esquerda) < 0) {
            // Caso Esquerda-Direita (LR)
            return rotacaoEsquerdaDireita(no);
        }
        // Caso Esquerda-Esquerda (LL)
        return rotacaoDireita(no);
    }

    // Caso Pensa para a Direita (FB < -1)
    if (fb < -1) {
        if (obterFatorBalanceamento(no->direita) > 0) {
            // Caso Direita-Esquerda (RL)
            return rotacaoDireitaEsquerda(no);
        }
        // Caso Direita-Direita (RR)
        return rotacaoEsquerda(no);
    }

    return no;
}

// Função para inserir com rebalanceamento AVL
No* inserir(No* raiz, int valor) {
    if (raiz == NULL) {
        return criarNo(valor);
    }

    if (valor < raiz->valor) {
        raiz->esquerda = inserir(raiz->esquerda, valor);
    } else if (valor > raiz->valor) {
        raiz->direita = inserir(raiz->direita, valor);
    } else {
        printf("Valor %d já existe na árvore. Ignorado.\n", valor);
        return raiz;
    }

    // Rebalanceia no caminho de volta
    return rebalancear(raiz);
}

// Encontra o menor nó da subárvore direita (sucessor in-order)
No* obterMenorNo(No* raiz) {
    No* atual = raiz;
    while (atual && atual->esquerda != NULL) {
        atual = atual->esquerda;
    }
    return atual;
}

// Função para remover com rebalanceamento AVL
No* remover(No* raiz, int valor) {
    if (raiz == NULL) {
        return NULL;
    }

    if (valor < raiz->valor) {
        raiz->esquerda = remover(raiz->esquerda, valor);
    } else if (valor > raiz->valor) {
        raiz->direita = remover(raiz->direita, valor);
    } else {
        // Casos 1 e 2: Sem filho ou 1 único filho
        if (raiz->esquerda == NULL || raiz->direita == NULL) {
            No* temp = raiz->esquerda ? raiz->esquerda : raiz->direita;

            if (temp == NULL) {
                // Caso Nó Folha
                temp = raiz;
                raiz = NULL;
            } else {
                // Caso 1 Filho
                *raiz = *temp;
            }
            free(temp);
        } else {
            // Caso 3: Nó com 2 Filhos
            No* temp = obterMenorNo(raiz->direita);
            raiz->valor = temp->valor;
            raiz->direita = remover(raiz->direita, temp->valor);
        }
    }

    if (raiz == NULL) return NULL;

    // Rebalanceia após a remoção
    return rebalancear(raiz);
}

// Função de busca
No* buscar(No* raiz, int valor) {
    if (raiz == NULL || raiz->valor == valor)
        return raiz;
    if (valor > raiz->valor)
        return buscar(raiz->direita, valor);
    return buscar(raiz->esquerda, valor);
}

// Percursos
void preOrdem(No* raiz) {
    if (raiz != NULL) {
        printf("%d ", raiz->valor);
        preOrdem(raiz->esquerda);
        preOrdem(raiz->direita);
    }
}

void emOrdem(No* raiz) {
    if (raiz != NULL) {
        emOrdem(raiz->esquerda);
        printf("%d ", raiz->valor);
        emOrdem(raiz->direita);
    }
}

void posOrdem(No* raiz) {
    if (raiz != NULL) {
        posOrdem(raiz->esquerda);
        posOrdem(raiz->direita);
        printf("%d ", raiz->valor);
    }
}

// Exibe a altura e o fator de balanceamento de cada nó 
void exibirEstadoAVL(No* raiz) {
    if (raiz != NULL) {
        exibirEstadoAVL(raiz->esquerda); 
        printf("Nó %d -> Altura: %d | FB: %d\n", raiz->valor, raiz->altura, obterFatorBalanceamento(raiz));
        exibirEstadoAVL(raiz->direita);  
    }
}

// Liberação de Memória
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
        printf("\nMenu:\n1. Inserir valor\n2. Buscar valor\n3. Remover valor\n4. Percorrer arvore\n5. Exibir altura e fator de balanceamento\n0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 0:
                printf("Liberando memória e saindo...\n");
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
                    printf("Valor %d encontrado na árvore.\n", valor);
                else
                    printf("Valor %d não encontrado.\n", valor);
                break;

            case 3:
                printf("Digite o valor a ser removido: ");
                scanf("%d", &valor);
                if (buscar(raiz, valor) != NULL) {
                    raiz = remover(raiz, valor);
                    printf("Valor %d removido da árvore.\n", valor);
                } else {
                    printf("Valor %d não encontrado.\n", valor);
                }
                break;

            case 4:
                if (raiz == NULL) {
                    printf("Árvore vazia!\n");
                    break;
                }
                printf("\n1. Pré-ordem\n2. Em ordem\n3. Pós-ordem\n");
                printf("Escolha uma opção: "); 
                scanf("%d", &opcao1);

                switch (opcao1) {
                    case 1:
                        printf("Pré-ordem: ");
                        preOrdem(raiz);
                        printf("\n");
                        break;
                    case 2:
                        printf("Em ordem: ");
                        emOrdem(raiz);
                        printf("\n");
                        break;
                    case 3:
                        printf("Pós-ordem: ");
                        posOrdem(raiz);
                        printf("\n");
                        break;
                    default:
                        printf("Opção de percurso inválida!\n");
                        break;
                }
                break;

            case 5:
                if (raiz == NULL) {
                    printf("Árvore vazia!\n");
                } else {
                    printf("\nAltura total da árvore: %d\n", obterAltura(raiz));
                    printf("--- Fator de Balanceamento dos Nós ---\n");
                    exibirEstadoAVL(raiz);
                }
                break;

            default:
                printf("Opção inválida!\n");
        }
    } while(opcao != 0);
}

int main() {
    setlocale(LC_ALL, "pt_BR.UTF-8");
    menu();
    return 0;
}