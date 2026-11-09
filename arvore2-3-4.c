#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
 
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
// Protótipos Corrigidos
Arvore* criarArvore();
No* criarNo(int folha);
void dividirFilho(No* pai, int i);
void inserir(Arvore* t, int valor);
No* buscar(No* atual, int valor);
void emOrdem(No* n);
void exibirPorNiveis(No* n);
int obterAltura(No* n);
void liberarNos(No* n);
void liberarArvore(Arvore* t);
void menu();
int lerInt(const char* msg, int* destino);
void removerDaArvore(Arvore* t, int valor);
// Funções auxiliares para remoção na Árvore 2-3-4
void removerNo(No* atual, int valor);
void removerDeFolha(No* atual, int idx);
void removerDeNaoFolha(No* atual, int idx);
int obterPredecessor(No* atual, int idx);
int obterSucessor(No* atual, int idx);
void preencher(No* atual, int idx);
void pegarEmprestadoAnterior(No* atual, int idx);
void pegarEmprestadoProximo(No* atual, int idx);
void fundir(No* atual, int idx);

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
void removerDaArvore(Arvore* t, int valor) {
    if (!t->raiz) {
        printf("A árvore está vazia.\n");
        return;
    }
    removerNo(t->raiz, valor);

    // Se a raiz ficar vazia após a remoção
    if (t->raiz->n == 0) {
        No* tmp = t->raiz;
        if (t->raiz->folha)
            t->raiz = NULL;
        else
            t->raiz = t->raiz->filhos[0];
        free(tmp);
    }
}

int encontrarChave(No* atual, int valor) {
    int idx = 0;
    while (idx < atual->n && atual->chaves[idx] < valor)
        ++idx;
    return idx;
}

void removerNo(No* atual, int valor) {
    int idx = encontrarChave(atual, valor);

    if (idx < atual->n && atual->chaves[idx] == valor) {
        if (atual->folha)
            removerDeFolha(atual, idx);
        else
            removerDeNaoFolha(atual, idx);
    } else {
        if (atual->folha) {
            printf("O valor %d não está na árvore.\n", valor);
            return;
        }
        int flag = ((idx == atual->n) ? 1 : 0);
        if (atual->filhos[idx]->n < 2)
            preencher(atual, idx);
        
        if (flag && idx > atual->n)
            removerNo(atual->filhos[idx - 1], valor);
        else
            removerNo(atual->filhos[idx], valor);
    }
}

void removerDeFolha(No* atual, int idx) {
    for (int i = idx + 1; i < atual->n; ++i)
        atual->chaves[i - 1] = atual->chaves[i];
    atual->n--;
}

void removerDeNaoFolha(No* atual, int idx) {
    int valor = atual->chaves[idx];
    if (atual->filhos[idx]->n >= 2) {
        int pred = obterPredecessor(atual, idx);
        atual->chaves[idx] = pred;
        removerNo(atual->filhos[idx], pred);
    } else if (atual->filhos[idx + 1]->n >= 2) {
        int suc = obterSucessor(atual, idx);
        atual->chaves[idx] = suc;
        removerNo(atual->filhos[idx + 1], suc);
    } else {
        fundir(atual, idx);
        removerNo(atual->filhos[idx], valor);
    }
}

int obterPredecessor(No* atual, int idx) {
    No* cur = atual->filhos[idx];
    while (!cur->folha)
        cur = cur->filhos[cur->n];
    return cur->chaves[cur->n - 1];
}

int obterSucessor(No* atual, int idx) {
    No* cur = atual->filhos[idx + 1];
    while (!cur->folha)
        cur = cur->filhos[0];
    return cur->chaves[0];
}

void preencher(No* atual, int idx) {
    if (idx != 0 && atual->filhos[idx - 1]->n >= 2)
        pegarEmprestadoAnterior(atual, idx);
    else if (idx != atual->n && atual->filhos[idx + 1]->n >= 2)
        pegarEmprestadoProximo(atual, idx);
    else {
        if (idx != atual->n)
            fundir(atual, idx);
        else
            fundir(atual, idx - 1);
    }
}

void pegarEmprestadoAnterior(No* atual, int idx) {
    No* filho = atual->filhos[idx];
    No* irmao = atual->filhos[idx - 1];

    for (int i = filho->n - 1; i >= 0; --i)
        filho->chaves[i + 1] = filho->chaves[i];

    if (!filho->folha) {
        for (int i = filho->n; i >= 0; --i)
            filho->filhos[i + 1] = filho->filhos[i];
    }

    filho->chaves[0] = atual->chaves[idx - 1];
    if (!filho->folha)
        filho->filhos[0] = irmao->filhos[irmao->n];

    atual->chaves[idx - 1] = irmao->chaves[irmao->n - 1];
    filho->n += 1;
    irmao->n -= 1;
}

void pegarEmprestadoProximo(No* atual, int idx) {
    No* filho = atual->filhos[idx];
    No* irmao = atual->filhos[idx + 1];

    filho->chaves[filho->n] = atual->chaves[idx];
    if (!filho->folha)
        filho->filhos[filho->n + 1] = irmao->filhos[0];

    atual->chaves[idx] = irmao->chaves[0];

    for (int i = 1; i < irmao->n; ++i)
        irmao->chaves[i - 1] = irmao->chaves[i];

    if (!irmao->folha) {
        for (int i = 1; i <= irmao->n; ++i)
            irmao->filhos[i - 1] = irmao->filhos[i];
    }
    filho->n += 1;
    irmao->n -= 1;
}

void fundir(No* atual, int idx) {
    No* filho = atual->filhos[idx];
    No* irmao = atual->filhos[idx + 1];

    filho->chaves[1] = atual->chaves[idx];

    for (int i = 0; i < irmao->n; ++i)
        filho->chaves[i + 2] = irmao->chaves[i];

    if (!filho->folha) {
        for (int i = 0; i <= irmao->n; ++i)
            filho->filhos[i + 2] = irmao->filhos[i];
    }

    for (int i = idx + 1; i < atual->n; ++i)
        atual->chaves[i - 1] = atual->chaves[i];

    for (int i = idx + 2; i <= atual->n; ++i)
        atual->filhos[i - 1] = atual->filhos[i];

    filho->n += irmao->n + 1;
    atual->n--;
    free(irmao);
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
    
    // Fila simples implementada com vetor (CORREÇÃO: agora cresce sozinha, sem limite fixo de 1000)
    int capacidade = 1024;
    No** fila = (No**)malloc(capacidade * sizeof(No*));
    if (fila == NULL) {
        printf("Erro de alocação de memória!\n");
        return;
    }
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
                        // Se a fila encheu, dobra a capacidade
                        if (fim >= capacidade) {
                            capacidade *= 2;
                            No** nova = (No**)realloc(fila, capacidade * sizeof(No*));
                            if (nova == NULL) {
                                free(fila);
                                printf("Erro de alocação de memória!\n");
                                return;
                            }
                            fila = nova;
                        }
                        fila[fim++] = atual->filhos[i];
                    }
                }
            }
            tamanhoNivel--;
        }
        printf("\n");
        nivel++;
    }
    free(fila);
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

// CORREÇÃO: leitura segura de inteiro (evita loop infinito quando o usuário digita letra)
// Retorna 1 se leu um número válido, 0 se a entrada acabou (EOF)
int lerInt(const char* msg, int* destino) {
    char linha[64];
    while (1) {
        printf("%s", msg);
        if (fgets(linha, sizeof(linha), stdin) == NULL) return 0;
        char* fim;
        long v = strtol(linha, &fim, 10);
        // leu pelo menos um dígito, não sobrou lixo e cabe em um int
        if (fim != linha && (*fim == '\n' || *fim == '\0') && v >= -2147483647L - 1 && v <= 2147483647L) {
            *destino = (int)v;
            return 1;
        }
        printf("Entrada inválida! Digite um número inteiro.\n");
    }
}

// MENU INTERATIVO 
void menu() {
    Arvore* t = criarArvore();
    int opcao, valor;

    do {
        printf("\nMenu:\n1. Inserir valor\n2. Buscar valor\n3. Remover valor\n4. Percorrer em ordem\n5. Exibir árvore por níveis\n0. Sair\n");
        if (!lerInt("Escolha uma opção: ", &opcao)) opcao = 0; // EOF: sai do programa

        switch(opcao) {
            case 0:
                printf("Liberando memória e saindo...\n");
                liberarArvore(t);
                t = NULL;
                break;

            case 1:
                if (!lerInt("Digite o valor a ser inserido: ", &valor)) break;
                inserir(t, valor);
                break;

            case 2:
                if (!lerInt("Digite um valor a ser buscado: ", &valor)) break;
                if (buscar(t->raiz, valor) != NULL)
                    printf("Valor %d encontrado na árvore.\n", valor);
                else
                    printf("Valor %d não encontrado.\n", valor);
                break;

           case 3:
                if (!lerInt("Digite o valor a ser removido: ", &valor)) break;
                // OBSERVAÇÃO: Função reescrita para fazer a remoção correta B-Tree
                if (buscar(t->raiz, valor) != NULL) {
                    removerDaArvore(t, valor);
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
                emOrdem(t->raiz);
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