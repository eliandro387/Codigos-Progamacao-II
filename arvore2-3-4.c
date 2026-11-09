#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
 
/*
Cada nó guarda de 1 a 3 chaves (ordenadas) e, se não for folha, n+1 filhos.
Todas as folhas ficam sempre no mesmo nível.
Valores repetidos: SÃO IGNORADOS!!!!! (a inserção avisa e não altera a árvore).*/
/*Utilizar struct para o nó da árvore, com um vetor de até três chaves, 
um vetor de até quatro ponteiros para filhos e um campo com a quantidade de chaves em uso.*/
// Estrutura do Nó da Árvore 
typedef struct No {
    int chaves[3];
    struct No *filhos[4];
    int n; // quantidade de chaves em uso
    int folha; // 1 se for folha 0 caso contrário
} No;

// A árvore guarda a raiz
typedef struct {
    No *raiz;
} Arvore;

// Protótipos
Arvore* criarArvore();
No* criarNo(int folha);
void dividirFilho(No* pai, int i);
void inserir(Arvore* t, int valor);
No* buscar(No* atual, int valor);
No* obterMenorNo(Arvore* t, No* n);
void remover(Arvore* t, No* z);
void emOrdem(No* n);
void exibirPorNiveis(No* n);
int obterAltura(Arvore* t, No* n);
void liberarNos(Arvore* t, No* n);
void liberarArvore(Arvore* t);
void menu();

// Retorna o maior valor entre dois inteiros
int maior(int a, int b) {
    return (a > b) ? a : b;
}
/*Implementar cada operação da árvore em funções separadas. 
A divisão de nó também deve ficar em função própria.*/
Arvore* criarArvore() {
    Arvore* t = (Arvore*)malloc(sizeof(Arvore));
    if (t == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    t->raiz = NULL;
    return t;
}

// Função para alocar dinamicamente um novo nó
No* criarNo(int folha) {
    No* novoNo = (No*)malloc(sizeof(No));
    if (novoNo == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    novoNo->n = 0;
    novoNo->folha = folha;
    for(int i = 0; i < 4; i++) novoNo->filhos[i] = NULL;
    return novoNo;
}

/*
1. Inserção com divisão antecipada. 
Inserir sempre em folha, dividindo durante a descida todo nó com três chaves encontrado no caminho e promovendo a chave do meio para o pai.
Quando a raiz for dividida, deve surgir uma nova raiz.
Caso valores repetidos sejam ignorados ou tratados de outro modo, isso deve ser documentado no código.
*/
void dividirFilho(No* pai, int i){
    No* y = pai->filhos[i]; // filho cheio vai ser dividido
    No* z = criarNo(y->folha); // novo nó irmão
 
    z->chaves[0] = y->chaves[2];
    z->n = 1;
    if (!y->folha) { // Se y for um nó interno, ele tem 4 filhos, é preciso distribuí-los. Se y for folha, não há filhos para mover.
        z->filhos[0] = y->filhos[2];
        z->filhos[1] = y->filhos[3];
    }
    int meio = y->chaves[1]; //Guarda a chave do meio que vai subir
    y->n = 1;
 
    // abre espaço no pai
    for (int j = pai->n; j > i; j--) {
        pai->chaves[j] = pai->chaves[j - 1];
    }
    for (int j = pai->n + 1; j > i + 1; j--) {
        pai->filhos[j] = pai->filhos[j - 1];
    } 
    pai->chaves[i] = meio; //Coloca a chave do meio na posição i do pai, pendura z logo à direita de y
    pai->filhos[i + 1] = z;
    pai->n++;
}

void inserir(Arvore* t, int valor) {
    // Valores repetidos são ignorados
    // OBSERVAÇÃO: Corrigida a chamada da busca para passar t->raiz
    if (buscar(t->raiz, valor) != NULL) {
        printf("Valor %d já existe na árvore. Ignorado.\n", valor);
        return;
    }
    
    if (t->raiz == NULL) {
        t->raiz = criarNo(1);
        t->raiz->chaves[0] = valor;
        t->raiz->n = 1;
        return;
    }
 
    if (t->raiz->n == 3) {
        No* nova = criarNo(0);
        nova->filhos[0] = t->raiz;
        t->raiz = nova;
        dividirFilho(nova, 0);
    }
    
    No* atual = t->raiz;
    while (!atual->folha) {
        int i = 0;
        while (i < atual->n && valor > atual->chaves[i]) i++;
        if (atual->filhos[i]->n == 3) {
            dividirFilho(atual, i);
            if (valor > atual->chaves[i]) i++;
        }
        atual = atual->filhos[i];
    }
 
    int i = atual->n - 1;
    while (i >= 0 && atual->chaves[i] > valor) {
        atual->chaves[i + 1] = atual->chaves[i];
        i--;
    }
    atual->chaves[i + 1] = valor;
    atual->n++;
}

/*Remover a chave mantendo todas as folhas no mesmo nível, emprestando chave de um irmão 
ou fundindo nós quando um nó ficar com chaves insuficientes.*/
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
No* buscar(No* atual, int valor) {
    if (atual == NULL) return NULL;
    
    int i = 0;
    while (i < atual->n && valor > atual->chaves[i]) {
        i++;
    }
    
    // Se encontrou a chave neste nó
    if (i < atual->n && valor == atual->chaves[i]) {
        return atual;
    }
    
    // Se for folha e não encontrou, o valor não existe
    if (atual->folha) {
        return NULL;
    }
    
    // Desce para o filho apropriado
    return buscar(atual->filhos[i], valor);
}

//Só pede um percurso
void emOrdem(No* n) {
    if (n == NULL) return;
    for (int i = 0; i < n->n; i++) {
        if (!n->folha) emOrdem(n->filhos[i]);
        printf("%d ", n->chaves[i]);
    }
    if (!n->folha) emOrdem(n->filhos[n->n]);
}
  
void exibirPorNiveis(No* raiz) {
    if (raiz == NULL) {
        printf("Árvore vazia!\n");
        return;
    }
    
    // Fila simples implementada com vetor
    No* fila[1000]; 
    int inicio = 0, fim = 0;
    
    fila[fim++] = raiz;
    int nivel = 0;
    
    while (inicio < fim) {
        int tamanhoNivel = fim - inicio;
        printf("Nível %d: ", nivel);
        
        while (tamanhoNivel > 0) {
            No* atual = fila[inicio++];
            
            // Imprime as chaves do nó atual
            printf("[");
            for (int i = 0; i < atual->n; i++) {
                printf("%d", atual->chaves[i]);
                if (i < atual->n - 1) printf(",");
            }
            printf("] ");
            
            // Adiciona filhos na fila
            if (!atual->folha) {
                for (int i = 0; i <= atual->n; i++) {
                    if (atual->filhos[i] != NULL) {
                        fila[fim++] = atual->filhos[i];
                    }
                }
            }
            tamanhoNivel--;
        }
        printf("\n");
        nivel++;
    }
}

// Altura  = número de níveis (todas as folhas estão no mesmo nível)
int obterAltura(No *n) {
    int h = 0;
    while (n != NULL) {
        h++;
        if (n->folha) break;
        n = n->filhos[0];
    }
    return h;
}

// Liberação de memória dos nós
void liberarNos(No* n) {
    if (n == NULL) return;
    if (!n->folha) {
        for (int i = 0; i <= n->n; i++) liberarNos(n->filhos[i]);
    }
    free(n);
}

// Liberação de Memória
void liberarArvore(Arvore* t) {
    liberarNos(t->raiz);
    free(t);
}

// MENU INTERATIVO 
void menu() {
    Arvore* t = criarArvore();
    int opcao, valor;

    do {
        printf("\nMenu:\n1. Inserir valor\n2. Buscar valor\n3. Remover valor\n4. Percorrer em ordem\n5. Exibir árvore por níveis\n0. Sair\n");
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
                if (buscar(t->raiz, valor) != NULL)
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
                if (t->raiz == NULL) {
                    printf("Árvore vazia!\n");
                    break;
                }
                printf("Em ordem: ");
                emOrdem(t, t->raiz);
                printf("\n");
                break;

           case 5:
                // OBSERVAÇÃO: Chamada da nova função criada
                exibirPorNiveis(t->raiz);
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
