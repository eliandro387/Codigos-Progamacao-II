#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define ALFABETO 26
#define MAX_PALAVRA 64

typedef struct no {
    struct no *filhos[ALFABETO]; //uma posição por letra 
    int fim;                     //1 se o caminho da raiz até aqui é uma palavra 
} No;

No *novo_no(void) {
    No *no = calloc(1, sizeof(No)); //calloc: filhos = NULL, fim = 0
    if (no == NULL) {
        fprintf(stderr, "Erro: sem memória.\n");
        exit(1);
    }
    return no;
}

int  validar_texto(const char *s);
int  inserir(No *raiz, const char *palavra);
No  *descer(No *raiz, const char *prefixo);
int  buscar(No *raiz, const char *palavra);
int  listar_com_prefixo(No *raiz, const char *prefixo);
int  remover(No *raiz, const char *palavra);
void exibir_arvore(No *raiz);
void liberar(No *no);
static void percorrer_em_ordem(No *no, char *buffer, int tam, int *contador);
static int  possui_filhos(No *no);
static int  excluir_rec(No *no, const char *palavra, int pos, int *removida);
static void exibir_rec(No *no, const char *indent);
static void ler_entrada(const char *rotulo, char *destino);
void menu(void);

//validar a palavra

int validar_texto(const char *s) {
    if (s[0] == '\0') return 0; //recusa a string vazia
    for (int i = 0; s[i] != '\0'; i++)
        if (s[i] < 'a' || s[i] > 'z') return 0;//e qualquer caractere fora do intervalo 'a'..'z'
    return 1;
}

//retorna 1 se inseriu, 0 se a palavra já existia
int inserir(No *raiz, const char *palavra) {
    No *atual = raiz;
    for (int i = 0; palavra[i] != '\0'; i++) { //percorre letra por letra da palavra
        int idx = palavra[i] - 'a';
        if (atual->filhos[idx] == NULL)           //caminho não existe: cria nó
            atual->filhos[idx] = novo_no();
        atual = atual->filhos[idx];               //caminho existe: apenas desce
    }
    if (atual->fim) return 0; //a palavra já existia
    atual->fim = 1; // liga o fim e retorna 1
    return 1;
}

//busca
//desce pelo prefixo; retorna o nó final ou NULL se o caminho não existe
No *descer(No *raiz, const char *prefixo) {
    No *atual = raiz;
    for (int i = 0; prefixo[i] != '\0'; i++) {
        int idx = prefixo[i] - 'a';
        if (atual->filhos[idx] == NULL) return NULL;
        atual = atual->filhos[idx]; //apenas desce
    }
    return atual;
}

//palavra presente: o caminho existe E o marcador de fim está ligado
int buscar(No *raiz, const char *palavra) {
    No *no = descer(raiz, palavra);
    return no != NULL && no->fim;
}

//busca por prefixo
//percorre a subárvore em ordem alfabética montando as palavras em buffer
static void percorrer_em_ordem(No *no, char *buffer, int tam, int *contador) {
    if (no->fim) { //se o no atual tiver fim ligado
        buffer[tam] = '\0';
        printf("  %s\n", buffer); // imprime a palavra e adiciona no contador
        (*contador)++;
    }
    for (int i = 0; i < ALFABETO; i++) {
        if (no->filhos[i] != NULL) {
            buffer[tam] = (char)('a' + i);
            percorrer_em_ordem(no->filhos[i], buffer, tam + 1, contador);
        }
    }
}

//lista todas as palavras que começam com o prefixo, retorna quantas
int listar_com_prefixo(No *raiz, const char *prefixo) {
    No *no = descer(raiz, prefixo);
    int contador = 0;
    if (no == NULL) return 0;

    char buffer[MAX_PALAVRA + ALFABETO];
    strcpy(buffer, prefixo); //copia o prefixo para o buffer
    percorrer_em_ordem(no, buffer, (int)strlen(prefixo), &contador);
    return contador;
}

//remoção
static int possui_filhos(No *no) { //retorna 1 se o nó tem algum filho, 0 caso contrário
    for (int i = 0; i < ALFABETO; i++)
        if (no->filhos[i] != NULL) return 1;
    return 0;
}

/*Desmarca o fim da palavra e apaga, na volta da recursão, os nós que
 * ficaram sem filhos e que não marcam o fim de outra palavra.
 * Retorna 1 se o nó 'no' pode ser apagado pelo chamador.
 * 'removida' recebe 1 se a palavra existia*/
static int excluir_rec(No *no, const char *palavra, int pos, int *removida) {
    if (palavra[pos] == '\0') {
        if (!no->fim) return 0;   //o caminho existe, mas não é palavra 
        no->fim = 0;              //desmarca o fim 
        *removida = 1;
        return !possui_filhos(no);   //apagável se ficou sem filhos 
    }

    int idx = palavra[pos] - 'a';
    if (no->filhos[idx] == NULL) return 0;  //palavra ausente 

    if (excluir_rec(no->filhos[idx], palavra, pos + 1, removida)) {
        free(no->filhos[idx]);
        no->filhos[idx] = NULL;
        return !no->fim && !possui_filhos(no);
    }
    return 0;
}

int remover(No *raiz, const char *palavra) {
    int removida = 0;
    excluir_rec(raiz, palavra, 0, &removida); //a raiz nunca é apagada
    return removida;
}

//desenho da árvore
/*Desenha a trie no terminal. Cada linha é um nó (a letra da ligação que
 * chega nele); um '*' marca o fim de palavra. 'indent' guarda o recuo e as
 * barras verticais herdadas dos ancestrais*/
static void exibir_rec(No *no, const char *indent) {
    int ultimo = -1;
    for (int i = 0; i < ALFABETO; i++)
        if (no->filhos[i] != NULL) ultimo = i;

    for (int i = 0; i < ALFABETO; i++) {
        if (no->filhos[i] == NULL) continue;
        int eh_ultimo = (i == ultimo);
        printf("%s%s %c%s\n", indent, eh_ultimo ? "└──" : "├──",
               'a' + i, no->filhos[i]->fim ? " *" : "");

        char novo[256];
        snprintf(novo, sizeof(novo), "%s%s", indent,
                 eh_ultimo ? "    " : "│   ");
        exibir_rec(no->filhos[i], novo);
    }
}

void exibir_arvore(No *raiz) {
    printf("(raiz)\n");
    if (!possui_filhos(raiz)) {
        printf("  (trie vazia)\n");
        return;
    }
    exibir_rec(raiz, "");
    printf("(* = fim de palavra)\n");
}

static void ler_entrada(const char *rotulo, char *destino) {
    printf("%s", rotulo);
    if (scanf("%63s", destino) != 1) {
        destino[0] = '\0';
    }
}

void liberar(No *no) {
    if (no == NULL) return;
    for (int i = 0; i < ALFABETO; i++)
        liberar(no->filhos[i]);
    free(no);
}

void menu(){
    No *raiz = novo_no();
    char texto[MAX_PALAVRA];
    int opcao;

    do {
        printf("\nMenu:\n1. Inserir palavra\n2. Buscar palavra\n3. Buscar prefixo\n4. Remover palavra\n5. Listar todas as palavras\n6. Desenhar a árvore\n0. Sair\n");
        printf("Escolha uma opção: ");

            if (scanf("%d", &opcao) != 1) { //entrada não numérica: descarta a linha e volta ao menu
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {}
                if (c == EOF) break; //fim da entrada: sai liberando a memória
                else { printf("Opção inválida.\n"); opcao = -1; 
                continue; 
            }
            }
        switch(opcao) {
            case 0: 
                printf("Liberando memória e saindo...\n");
                liberar(raiz);
                break;

            case 1:
                ler_entrada("Palavra: ", texto);
                if (!validar_texto(texto))
                    printf("Use apenas letras minúsculas de a a z.\n");
                else if (inserir(raiz, texto))
                    printf("\"%s\" inserida.\n", texto);
                else
                    printf("\"%s\" já estava na trie.\n", texto);
                break;

            case 2:
                ler_entrada("Palavra: ", texto);
                if (!validar_texto(texto))
                    printf("Use apenas letras minúsculas de a a z.\n");
                else if (buscar(raiz, texto))
                    printf("\"%s\" encontrada.\n", texto);
                else if (descer(raiz, texto) != NULL)
                    printf("\"%s\" não é palavra (existe só como prefixo).\n", texto);
                else
                    printf("\"%s\" não encontrada.\n", texto);
                break;

            case 3:
                ler_entrada("Prefixo: ", texto);
                if (!validar_texto(texto))
                    printf("Use apenas letras minúsculas de a a z.\n");
                else {
                    printf("Palavras com prefixo \"%s\":\n", texto);
                    int n = listar_com_prefixo(raiz, texto);
                    if (n == 0) printf("  (nenhuma)\n");
                    else printf("Total: %d\n", n);
                }
                break;

            case 4:
                ler_entrada("Palavra: ", texto);
                if (!validar_texto(texto))
                    printf("Use apenas letras minúsculas de a a z.\n");
                else if (remover(raiz, texto))
                    printf("\"%s\" removida.\n", texto);
                else
                    printf("\"%s\" não está na trie.\n", texto);
                break;

         case 5: 
            printf("Todas as palavras:\n");
            char buffer[MAX_PALAVRA + ALFABETO];
            int n = 0;
            percorrer_em_ordem(raiz, buffer, 0, &n);
            if (n == 0) printf("  (trie vazia)\n");
            else printf("Total: %d\n", n);
            break;

        case 6:
            exibir_arvore(raiz);
            break;

        default:
            printf("Opção inválida!\n");
        }
    } while(opcao != 0);
}

int main(void) {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif
    setlocale(LC_ALL, "pt_BR.UTF-8");
    menu();
    return 0;
}
