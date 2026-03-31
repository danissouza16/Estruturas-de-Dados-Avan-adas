// Caio Nabuco Vogel - 2310878
// Daniel Santana Souza - 2310995

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct lista {
    int info;
    struct lista* prox;
};typedef struct lista Lista;

struct fila {
    Lista* ini;
    Lista* fim;
};typedef struct fila Fila;

Fila* fila_cria (void);
void fila_insere (Fila* f, int v);
int fila_retira (Fila* f);
void fila_libera (Fila* f);
int fila_vazia (Fila* f);
void imprime_fila(Fila* f);

int main(void) {
    Fila* f;
    f = fila_cria();
    FILE* arq;
    char linha[100];

    arq = fopen("entrada_fila.txt", "r");
    if(arq == NULL) {
        printf("Erro ao abrir entrada_fila.txt\n");
        exit(1);
    }

    while(fgets(linha, sizeof(linha), arq) != NULL) {
        for(int i = 0; i < strlen(linha); i++) {
            
            if(linha[i] == 'a') {
                char val[100];
                int j = 0;
                i++;
                while(linha[i] != ' ' && linha[i] != '\n' && linha[i] != '\0') {
                    val[j] = linha[i];
                    j++;
                    i++;
                }
                val[j] = '\0';
                fila_insere(f, atoi(val));
                imprime_fila(f);
            }
            else if(linha[i] == 'r') {
                fila_retira(f);
                imprime_fila(f);
            }
        }
        fila_libera(f);
        f = fila_cria();
    }
    fila_libera(f); 
    fclose(arq);
    return 0;
}

Fila* fila_cria (void) {
    Fila* f = (Fila*) malloc(sizeof(Fila));
    f->ini = NULL;
    f->fim = NULL;
    return f;
}

void fila_insere (Fila* f, int v) {
    Lista* n = (Lista*) malloc(sizeof(Lista));
        n->info = v;
        n->prox = NULL;
    if (f->fim != NULL) {
        f->fim->prox = n;
        f->fim = n;
    }
    else {
        f->ini = n;
        f->fim = n;
    }
}

int fila_retira (Fila* f) { 
    Lista* t;
    int v;
    if (fila_vazia(f)) {
        printf("Fila vazia.\n");
        exit(1); 
    } 
    t = f->ini;
    v = t->info;
    f->ini = t->prox;
    if (f->ini == NULL)
        f->fim = NULL;
    free(t);
    return v;
}

void imprime_fila(Fila* f) {
    if (fila_vazia(f)) {
        printf("[]\n");
        return;
    }
    
    printf("[");
    Lista* atual = f->ini;
    while(atual != NULL) {
        printf("%d", atual->info);
        if(atual->prox != NULL) {
            printf(", ");
        }
        atual = atual->prox;
    }
    printf("]\n");
}

void fila_libera (Fila* f) {
    Lista* q = f->ini;
    while (q!=NULL) {
        Lista* t = q->prox;
        free(q);
        q = t;
    }
    free(f);
}


int fila_vazia (Fila* f) {
    if(f->ini == NULL)
        return 1;
    return 0;
}