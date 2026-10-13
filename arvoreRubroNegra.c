#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

// Estrutura e cor do Nó da Árvore Rubro-Negra

typedef enum { VERMELHO, PRETO } Cor;
typedef struct No {
    int valor;
    Cor cor;
    struct No *esquerda;
    struct No *direita;
    struct No *pai;
} No;

// A árvore guarda a raiz e um nó "nil" (preto), usado no lugar de NULL
typedef struct {
    No *raiz;
    No *nil;
} Arvore;

// Protótipos
Arvore* criarArvore();
No* criarNo(Arvore* t, int valor);
void rotacaoEsquerda(Arvore* t, No* x);
void rotacaoDireita(Arvore* t, No* y);
void inserirCorrecao(Arvore* t, No* z);
void inserir(Arvore* t, int valor);
No* buscar(Arvore* t, int valor);
No* obterMenorNo(Arvore* t, No* n);
void transplantar(Arvore* t, No* u, No* v);
void removerCorrecao(Arvore* t, No* x);
void remover(Arvore* t, No* z);
void preOrdem(Arvore* t, No* n);
void emOrdem(Arvore* t, No* n);
void posOrdem(Arvore* t, No* n);
int obterAltura(Arvore* t, No* n);
int alturaNegra(Arvore* t, No* n);
void exibirEstadoRB(Arvore* t, No* n);
void liberarNos(Arvore* t, No* n);
void liberarArvore(Arvore* t);
void menu();

// Retorna o maior valor entre dois inteiros
int maior(int a, int b) {
    return (a > b) ? a : b;
}

Arvore* criarArvore() {
    Arvore* t = (Arvore*)malloc(sizeof(Arvore));
    if (t == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    t->nil = (No*)malloc(sizeof(No));
    if (t->nil == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    t->nil->valor =  0;
    t->nil->cor = PRETO;
    t->nil->esquerda = t->nil->direita = t->nil->pai = t->nil;
    t->raiz = t->nil;
    return t;
}

// Função para alocar dinamicamente um novo nó
No* criarNo(Arvore* t, int valor) {
    No* novoNo = (No*)malloc(sizeof(No));
    if (novoNo == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    novoNo->valor = valor;
    novoNo->cor = VERMELHO; // Todo nó novo começa com cor vermelha
    novoNo->esquerda = t->nil;
    novoNo->direita = t->nil;
    novoNo->pai = t->nil;
    return novoNo;
}

// 1. Rotação Simples à Direita
void rotacaoDireita(Arvore* t, No* y) {
    No* x = y->esquerda;

    y->esquerda = x->direita;
    if(x->direita != t->nil) {
        x->direita->pai = y;
    }

    x->pai = y->pai;
    if (y->pai == t->nil) {
        t->raiz = x;
    } else if (y == y->pai->esquerda) {
        y->pai->esquerda = x;
    } else {
        y->pai->direita = x;
    }
    x->direita = y;
    y->pai = x;
}

// 2. Rotação Simples à Esquerda
void rotacaoEsquerda(Arvore* t, No* x) {
    No *y = x->direita;
    x->direita = y->esquerda;
    if(y->esquerda != t->nil) {
        y->esquerda->pai = x;
    }

    y->pai= x->pai;
    if (x->pai == t->nil) {
        t->raiz = y;
    } else if (x == x->pai->esquerda) {
        x->pai->esquerda = y;
    } else {
        x->pai->direita = y;
    }
    y->esquerda = x;
    x->pai = y;
}

// Conserta violações (dois vermelhos seguidos) após inserir
void inserirCorrecao(Arvore*t, No* z) {
    while (z->pai->cor == VERMELHO) {
        if (z->pai == z->pai->pai->esquerda) {
            No* tio = z->pai->pai->direita;
            if (tio->cor == VERMELHO) {
                // Caso 1: tio vermelho -> recolorir e subir
                z->pai->cor = PRETO;
                tio->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                z = z->pai->pai;
            } else {
                if (z == z->pai->direita) {
                    // Caso 2: triângulo -> rotação para virar linha
                    z = z->pai;
                    rotacaoEsquerda(t, z);
                }
                // Caso 3: linha -> recolorir e rotacionar
                z->pai->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                rotacaoDireita(t, z->pai->pai);
            }
            } else {
            // Simétrico
            No* tio = z->pai->pai->esquerda;
            if (tio->cor == VERMELHO) {
                z->pai->cor = PRETO;
                tio->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                z = z->pai->pai;
            } else {
                if (z == z->pai->esquerda) {
                    z = z->pai;
                    rotacaoDireita(t, z);
                }
                z->pai->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                rotacaoEsquerda(t, z->pai->pai);
            }
        }
    }
    t->raiz->cor = PRETO; // a raiz é sempre preta
}
// Função para inserir
void inserir(Arvore* t, int valor) {
    No* pai = t->nil;
    No* atual = t->raiz;

    while (atual != t->nil) {
        pai = atual;
        if (valor < atual->valor) {
            atual = atual->esquerda;
        } else if (valor > atual->valor) {
            atual = atual->direita;
        } else {
            printf("Valor %d já existe na árvore. Ignorado.\n", valor);
            return;
        }
    }

    No* z = criarNo(t, valor);
    z->pai = pai;

    if (pai == t->nil) {
        t->raiz = z;
    } else if (valor < pai->valor) {
        pai->esquerda = z;
    } else {
        pai->direita = z;
    }

    z->cor = VERMELHO;
    inserirCorrecao(t, z);
}

// Encontra o menor nó da subárvore direita
No* obterMenorNo(Arvore* t, No* n) {
    while (n->esquerda != t->nil){
        n = n->esquerda;
    }
    return n;
}

// Substitui a subárvore u pela subárvore v
void transplantar(Arvore* t, No* u, No* v){
    if (u->pai == t->nil) {
        t->raiz = v;
    } else if (u == u->pai->esquerda) {
        u->pai->esquerda = v;
    } else {
        u->pai->direita = v;
    }
    v->pai = u->pai; // vale mesmo se v for o nil (necessário na correção)
}

// Conserta o "duplo preto" após remover um nó preto
void removerCorrecao(Arvore *t, No *x) {
    while (x != t->raiz && x->cor == PRETO) {
        if (x == x->pai->esquerda) {
            No *w = x->pai->direita; // irmão
            if (w->cor == VERMELHO) {
                // Caso 1: irmão vermelho
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                rotacaoEsquerda(t, x->pai);
                w = x->pai->direita;
            }
            if (w->esquerda->cor == PRETO && w->direita->cor == PRETO) {
                // Caso 2: irmão preto com sobrinhos pretos
                w->cor = VERMELHO;
                x = x->pai;
            } else {
                if (w->direita->cor == PRETO) {
                    // Caso 3: sobrinho distante preto
                    w->esquerda->cor = PRETO;
                    w->cor = VERMELHO;
                    rotacaoDireita(t, w);
                    w = x->pai->direita;
                }
                // Caso 4: sobrinho distante vermelho
                w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                w->direita->cor = PRETO;
                rotacaoEsquerda(t, x->pai);
                x = t->raiz;
            }
        } else {
            // Simétrico
            No *w = x->pai->esquerda;
            if (w->cor == VERMELHO) {
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                rotacaoDireita(t, x->pai);
                w = x->pai->esquerda;
            }
            if (w->direita->cor == PRETO && w->esquerda->cor == PRETO) {
                w->cor = VERMELHO;
                x = x->pai;
            } else {
                if (w->esquerda->cor == PRETO) {
                    w->direita->cor = PRETO;
                    w->cor = VERMELHO;
                    rotacaoEsquerda(t, w);
                    w = x->pai->esquerda;
                }
                w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                w->esquerda->cor = PRETO;
                rotacaoDireita(t, x->pai);
                x = t->raiz;
            }
        }
    }
    x->cor = PRETO;
}

// Função para remover
void remover(Arvore* t, No* z) {
    No* y = z;
    No* x;
    Cor corOriginal = y->cor;

    if (z->esquerda == t->nil) {
        x = z->direita;
        transplantar(t, z, z->direita);
    } else if (z->direita == t->nil) {
        x = z->esquerda;
        transplantar(t, z, z->esquerda);
    } else {
        // Nó com 2 filhos: usa o sucessor in-order
        y = obterMenorNo(t, z->direita);
        corOriginal = y->cor;
        x = y->direita;

        if (y->pai == z) {
            x->pai = y;
        } else {
            transplantar(t, y, y->direita);
            y->direita = z->direita;
            y->direita->pai = y;
        }
        transplantar(t, z, y);
        y->esquerda = z->esquerda;
        y->esquerda->pai = y;
        y->cor = z->cor;
    }
    free(z);

    // Só há violação se o nó removido/movido era preto
    if (corOriginal == PRETO) {
        removerCorrecao(t, x);
    }
    t->nil->pai = t->nil; // mantém o sentinela limpo
}

// Função de busca
No* buscar(Arvore* t, int valor) {
    No* atual = t->raiz;
    while (atual != t->nil && atual->valor != valor) {
        atual = (valor < atual->valor) ? atual->esquerda : atual->direita;
    }
    return atual;
}

// Percursos
void preOrdem(Arvore* t, No* n) {
    if (n != t->nil) {
        printf("%d[%s] ", n->valor, n->cor == VERMELHO ? "V" : "P");
        preOrdem(t, n->esquerda);
        preOrdem(t, n->direita);
    }
}

void emOrdem(Arvore* t, No* n) {
    if (n != t->nil) {
        emOrdem(t, n->esquerda);
        printf("%d[%s] ", n->valor, n->cor == VERMELHO ? "V" : "P");
        emOrdem(t, n->direita);
    }
}

void posOrdem(Arvore* t, No* n) {
    if (n != t->nil) {
        posOrdem(t, n->esquerda);
        posOrdem(t, n->direita);
        printf("%d[%s] ", n->valor, n->cor == VERMELHO ? "V" : "P");
    }
}

// Altura calculada na hora (a RB não armazena altura nos nós)
int obterAltura(Arvore *t, No *n) {
    if (n == t->nil) return 0;
    return 1 + maior(obterAltura(t, n->esquerda), obterAltura(t, n->direita));
}
 
// Altura negra: nº de nós pretos da raiz até uma folha (sem contar a raiz do nil)
int alturaNegra(Arvore *t, No *n) {
    if (n == t->nil) return 0;
    return alturaNegra(t, n->esquerda) + (n->cor == PRETO ? 1 : 0);
}
 
// Exibe cor, pai e altura negra de cada nó
void exibirEstadoRB(Arvore *t, No *n) {
    if (n != t->nil) {
        exibirEstadoRB(t, n->esquerda);
        printf("Nó %d -> Cor: %s | Pai: ", n->valor, n->cor == PRETO ? "PRETO" : "VERMELHO");
        if (n->pai == t->nil) printf("(raiz)");
        else printf("%d", n->pai->valor);
        printf(" | Altura negra: %d\n", alturaNegra(t, n));
        exibirEstadoRB(t, n->direita);
    }
}

// Liberação de memória dos nós
void liberarNos(Arvore* t, No* n) {
    if (n != t->nil) {
        liberarNos(t, n->esquerda);
        liberarNos(t, n->direita);
        free(n);
    }
}

// Liberação de Memória
void liberarArvore(Arvore* t) {
    liberarNos(t, t->raiz);
    free(t->nil);
    free(t->raiz);
}

// MENU INTERATIVO 
void menu() {
    Arvore* t = criarArvore();
    int opcao, valor, opcao1;

    do {
        printf("\nMenu:\n1. Inserir valor\n2. Buscar valor\n3. Remover valor\n4. Percorrer arvore\n5. Exibir cor e altura negra\n0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 0:
                printf("Liberando memória e saindo...\n");
                liberarArvore(t);
                t = NULL;
                break;

            case 1:
                printf("Digite o valor a ser inserido: ");
                scanf("%d", &valor);
                inserir(t, valor);
                break;

            case 2:
                printf("Digite um valor a ser buscado: ");
                scanf("%d", &valor);
                if (buscar(t, valor) != t->nil)
                    printf("Valor %d encontrado na árvore.\n", valor);
                else
                    printf("Valor %d não encontrado.\n", valor);
                break;

            case 3:
                printf("Digite o valor a ser removido: ");
                scanf("%d", &valor);
                No* alvo = buscar(t, valor);
                if (alvo != t->nil) {
                    remover(t, alvo);
                    printf("Valor %d removido da árvore.\n", valor);
                } else {
                    printf("Valor %d não encontrado.\n", valor);
                }
                break;

            case 4:
                if (t->raiz == t->nil) {
                    printf("Árvore vazia!\n");
                    break;
                }
                printf("\n1. Pré-ordem\n2. Em ordem\n3. Pós-ordem\n");
                printf("Escolha uma opção: "); 
                scanf("%d", &opcao1);

                switch (opcao1) {
                    case 1:
                        printf("Pré-ordem: ");
                        preOrdem(t, t->raiz);
                        printf("\n");
                        break;
                    case 2:
                        printf("Em ordem: ");
                        emOrdem(t, t->raiz);
                        printf("\n");
                        break;
                    case 3:
                        printf("Pós-ordem: ");
                        posOrdem(t, t->raiz);
                        printf("\n");
                        break;
                    default:
                        printf("Opção de percurso inválida!\n");
                        break;
                }
                break;

            case 5:
                if (t->raiz == t->nil) {
                    printf("Árvore vazia!\n");
                } else {
                    printf("\nAltura total da árvore: %d\n", obterAltura(t, t->raiz));
                    printf("--- Cor e altura negra dos nós ---\n");
                    exibirEstadoRB(t, t->raiz);
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
