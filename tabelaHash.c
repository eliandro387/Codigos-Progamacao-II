#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#define TAM 7 // tamanho da tabela (primo ajuda a espalhar melhor)

//nó da lista encadeada de cada posição
typedef struct No{
    int chave;
    struct No* prox;
} No;

//vetor de ponteiros para o início das listas
typedef struct{
    No* pos[TAM];
} TabelaHash;

//Protótipos das funções
void inicializar(TabelaHash *t);
No *buscar(TabelaHash *t, int chave);
void inserir(TabelaHash *t, int chave);
void remover(TabelaHash *t, int chave);
void imprimir(TabelaHash *t);
void liberar(TabelaHash *t);

//função hash
int hash(int chave){
    int h = chave % TAM;
    return h < 0 ? h + TAM : h; //chaves negativas
}

void inicializar(TabelaHash* t){
    for (int i = 0; i < TAM; i++)
        t->pos[i] = NULL;
}
//busca a chave na lista da posição calculada
No* buscar(TabelaHash* t, int chave){
    No* atual = t->pos[hash(chave)];
    while (atual != NULL){
        if(atual->chave == chave)
            return atual;
        atual = atual->prox;
    }
    return NULL;
}
//
void inserir(TabelaHash* t, int chave){
    if (buscar(t, chave) != NULL){
        printf("Chave %d já existe na tabela.\n", chave);
        return;
    }
    int i = hash(chave);
    No* novo = (No*)malloc(sizeof(No));
    if (novo == NULL){
        printf("Erro de alocação.\n");
        return;
    }
    if(t->pos[i] !=NULL)
        printf("Colisão na posição %d: chave %d encadeada na lista\n", i, chave);

    novo->chave = chave;
    novo->prox = t->pos[i]; // insere na cabeça da lista
    t->pos[i] = novo;
    printf("Chave %d inserida na posição %d\n", chave, i);
}
//percorre a lista da posição e religa os ponteiros
void remover(TabelaHash* t, int chave){
    int i = hash(chave);
    No* atual = t->pos[i];
    No* anterior = NULL;
    while (atual != NULL && atual->chave != chave){
        anterior = atual;
        atual = atual->prox;
    }
    if (atual == NULL){
        printf("Chave %d não encontrada.\n", chave);
        return;
    }
    if (anterior == NULL) //remover o primeiro da lista
        t->pos[i] = atual->prox;
    else
        anterior->prox = atual->prox;
    free(atual);
    printf("Chave %d removida da posição %d.\n", chave, i);
}

void imprimir(TabelaHash* t){
    for(int i = 0; i <TAM; i++){
        printf("[&2d]", i);
        for(No* p = t->pos[i]; p != NULL; p = p->prox)
            printf("%d -> ", p->chave);
        printf("NULL\n");
    }
}

void liberar(TabelaHash* t){
    for(int i = 0; i < TAM; i++){
        No* p = t->pos[i];
        while(p != NULL){
            No* prox = p->prox;
            free(p);
            p = prox;
        }
        t->pos[i] = NULL;
    }
}

void menu() {
    TabelaHash tabela;
    int opcao, chave;

    inicializar(&tabela);

    do {
        printf("\nMenu:\n1. Inserir\n2. Remover\n3. Buscar\n4. Imprimir tabela\n0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 0:
                printf("Liberando memória e saindo...\n");
                liberar(&tabela);
                break;

            case 1:
                printf("Digite a chave a ser inserida: ");
                scanf("%d", &chave);
                inserir(&tabela, chave);
                break;

            case 2:
                printf("Digite a chave ser removida: ");
                scanf("%d", &chave);
                remover(&tabela, chave);
                break;

            case 3:
                printf("Digite a chave a ser buscada: ");
                scanf("%d", &chave);
                if (buscar(&tabela, chave))
                    printf("Chave %d encontrada na posicao %d.\n", chave, hash(chave));
                else
                    printf("Chave %d nao encontrada.\n", chave);
                break;

            case 4:
                imprimir(&tabela);
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
