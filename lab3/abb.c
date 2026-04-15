// Daniel Santana Souza - 2310995

#include <stdio.h>
#include <stdlib.h>

struct nodo {
    int chave;
    int altura;
    struct nodo * esq;
    struct nodo * dir;
}; typedef struct nodo Nodo;

struct abb{
    Nodo * raiz;
}; typedef struct abb ABB;

Nodo* cria(int chav);
Nodo* insere(Nodo* raiz, int chave);
ABB *busca(Nodo *raiz, int chave);
int calcula_altura(Nodo *raiz);
void pre_ordem(Nodo *raiz);
void ordem_simetrica(Nodo *raiz);
void por_nivel(Nodo *raiz);
void liberar_arvore(Nodo *raiz);

int main() {
    ABB arvore;
    arvore.raiz = NULL;
    FILE* arq;
    arq = fopen( "entrada_abb.txt" , "r");

    if(arq==NULL) 
        exit(1);
    
    int chave;

    while (fscanf(arq, "%d", &chave) == 1) {
        arvore.raiz = insere(arvore.raiz, chave);
    }
    fclose(arq);

    calcula_altura(arvore.raiz);
    if (arvore.raiz != NULL) {
        printf("Pré-ordem: ");
        pre_ordem(arvore.raiz);
        printf("\n");
        
        printf("Ordem Simétrica: ");
        ordem_simetrica(arvore.raiz);
        printf("\n");
        
        printf("Por nível: ");
        por_nivel(arvore.raiz);
        printf("\n");
    }
    liberar_arvore(arvore.raiz);

    return 0;
}

Nodo* cria(int chav) {
    Nodo* no = (Nodo*) malloc(sizeof(Nodo));
    if (no != NULL) {
        no->chave = chav;
        no->altura = 0; 
        no->esq = NULL;
        no->dir = NULL;
    }
    return no;
}

Nodo* insere(Nodo* no, int chave) {
    if (no == NULL) {
        return cria(chave);
    }
    else if (chave < no->chave) {
        no->esq = insere(no->esq, chave);
    }
    else if (chave > no->chave) {
        no->dir = insere(no->dir, chave);
    }
    return no;
}

int calcula_altura(Nodo* no) {
    if (no == NULL) {
        return -1; 
    }

    int alt_esq = calcula_altura(no->esq);
    int alt_dir = calcula_altura(no->dir);
    
    if (alt_esq > alt_dir) {
        no->altura = 1 + alt_esq;
    } else {
        no->altura = 1 + alt_dir;
    }   
    return no->altura;
}

void visita(Nodo* no) {
    printf("%d(%d) ", no->chave, no->altura);
}


void pre_ordem(Nodo* no) {
    visita(no);
    if (no->esq != NULL) {
        pre_ordem(no->esq);
    }
    if (no->dir != NULL) {
        pre_ordem(no->dir);
    }
}

void ordem_simetrica(Nodo* no) {
    if (no->esq != NULL) {
        ordem_simetrica(no->esq);
    }
    visita(no);
    if (no->dir != NULL) {
        ordem_simetrica(no->dir);
    }
}


void por_nivel(Nodo* raiz) {
    if (raiz == NULL) return;
     
    Nodo* fila[200]; 
    int inicio = 0, fim = 0;
    fila[fim++] = raiz; 
    
    while (inicio < fim) {
        Nodo* atual = fila[inicio++]; 
        visita(atual);
        if (atual->esq != NULL) {
            fila[fim++] = atual->esq;
        }
        if (atual->dir != NULL) {
            fila[fim++] = atual->dir;
        }
    }
}

void liberar_arvore(Nodo* no) {
    if (no != NULL) {
        liberar_arvore(no->esq);
        liberar_arvore(no->dir);
        free(no);
    }
}