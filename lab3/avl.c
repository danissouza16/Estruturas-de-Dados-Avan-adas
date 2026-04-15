#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct nodo {
    int chave;
    int fb; /*fator de balanceamento*/
    struct nodo *esq;
    struct nodo *dir;
}; typedef struct nodo Nodo;

struct avl{
    Nodo * raiz;
}; typedef struct avl AVL;

char operacao_rotacao[500];

Nodo* cria(int chave) {
    Nodo* novo = (Nodo*)malloc(sizeof(Nodo));
    if (novo != NULL) {
        novo->chave = chave;
        novo->fb = 0;
        novo->esq = NULL;
        novo->dir = NULL;
    }
    return novo;
}

int calcula_altura(Nodo* no) {
    if (no == NULL) {
        return -1;
    }

    int alt_esq = calcula_altura(no->esq);
    int alt_dir = calcula_altura(no->dir);

    if (alt_esq > alt_dir) {
        alt_esq++;
        return alt_esq;
    } else {
        alt_dir++;
        return alt_dir;
    }   
}


void atualiza_fator(Nodo* no) {
    if (no != NULL) {
        no->fb = calcula_altura(no->dir) - calcula_altura(no->esq);
    }
}

//rotações
Nodo* rotacao_direita(Nodo* raiz) {
    char buf[100];
    sprintf(buf, "Rotação à direita (RD) no nó %d. ", raiz->chave);
    strcat(operacao_rotacao, buf);

    Nodo* t = raiz->esq;
    raiz->esq = t->dir;
    t->dir = raiz;
    
    atualiza_fator(raiz);
    atualiza_fator(t);
    return t;
}

Nodo* rotacao_esquerda(Nodo* raiz) {
    char buf[100];
    sprintf(buf, "Rotação à esquerda (RE) no nó %d. ", raiz->chave);
    strcat(operacao_rotacao, buf);

    Nodo* t = raiz->dir;
    raiz->dir = t->esq;
    t->esq = raiz;
    
    atualiza_fator(raiz);
    atualiza_fator(t);
    return t;
}

Nodo* rotacao_dupla_direita(Nodo* r) {
    char buf[100];
    sprintf(buf, "Rotação dupla à direita (RED) no nó %d. ", r->chave);
    strcat(operacao_rotacao, buf);

    Nodo* t = r->esq;
    Nodo* m = t->dir;

    t->dir = m->esq;
    m->esq = t;
    r->esq = m->dir;
    m->dir = r;

    atualiza_fator(t);
    atualiza_fator(r);
    atualiza_fator(m);
    return m;
}

Nodo* rotacao_dupla_esquerda(Nodo* raiz) {
    char buf[100];
    sprintf(buf, "Rotação dupla à esquerda (RDE) no nó %d. ", raiz->chave);
    strcat(operacao_rotacao, buf);

    Nodo* t = raiz->dir;
    Nodo* s = raiz->esq;

    t->esq = s->dir;
    s->dir = t;
    raiz->dir = s->esq;
    s->esq = raiz;

    atualiza_fator(t);
    atualiza_fator(raiz);
    atualiza_fator(s);
    return s;
}

Nodo* insere(Nodo* raiz, int chave) {
    if (raiz == NULL) {
        return cria(chave);
    }
    
    if (chave < raiz->chave) {
        raiz->esq = insere(raiz->esq, chave);
    } else if (chave > raiz->chave) {
        raiz->dir = insere(raiz->dir, chave);
    } else {
        return raiz;
    }

    atualiza_fator(raiz);

    if (raiz->fb == -2) { //pra esquerda
        if (raiz->esq != NULL && raiz->esq->fb <= 0) {
            return rotacao_direita(raiz);
        } else {
            return rotacao_dupla_direita(raiz);
        }
    }
    if (raiz->fb == 2) { //pra direita
        if (raiz->dir != NULL && raiz->dir->fb >= 0) {
            return rotacao_esquerda(raiz);
        } else {
            return rotacao_dupla_esquerda(raiz);
        }
    }
    
    return raiz;
}

Nodo* remove_n(Nodo* raiz, int chave) {
    if (raiz == NULL) {
        return NULL;
    } 

    Nodo* temp = (Nodo*)malloc(sizeof(Nodo));

    if (chave < raiz->chave) {
        raiz->esq = remove_n(raiz->esq, chave);
    } else if (chave > raiz->chave) {
        raiz->dir = remove_n(raiz->dir, chave);
    } else {
        if (raiz->esq == NULL || raiz->dir == NULL) { 
            if (raiz->esq) {
               Nodo* temp = raiz->esq;  
            } else {
                Nodo* temp = raiz->dir;
            }
            if (temp == NULL) {
                temp = raiz;
                raiz = NULL;
            } else {
                *raiz = *temp; 
            }
            free(temp);
        } else {
            //2 filhos: pega o sucessor (menor da direita)
            Nodo* temp = raiz->dir;
            while (temp->esq != NULL) temp = temp->esq;
            raiz->chave = temp->chave;
            raiz->dir = remove_n(raiz->dir, temp->chave);
        }
    }

    if (raiz == NULL) {
        return NULL;
    }  //vazia

    atualiza_fator(raiz);

    //balanceamento dps da remoção
    if (raiz->fb == -2) {
        if (raiz->esq != NULL && raiz->esq->fb <= 0) {
            return rotacao_direita(raiz);
        } else {
            return rotacao_dupla_direita(raiz);
        }
    }
    if (raiz->fb == 2) {
        if (raiz->dir != NULL && raiz->dir->fb >= 0) {
            return rotacao_esquerda(raiz);
        } else {
            return rotacao_dupla_esquerda(raiz);
        }
    }

    return raiz;
}

void pre_ordem(Nodo* r) {
    if (r != NULL) {
        printf("%d(fb:%d) ", r->chave, r->fb);
        pre_ordem(r->esq);
        pre_ordem(r->dir);
    }
}


int main() {
    AVL arvore;
    arvore.raiz = cria(50);
    
    int chaves_inserir[] = {1, 64, 12, 18, 66, 38, 95, 58, 59, 70, 43, 16, 67, 39};
    int num_inserir = sizeof(chaves_inserir) / sizeof(chaves_inserir[0]);

    printf("2A\n");
    for (int i = 0; i < num_inserir; i++) {
        operacao_rotacao[0] = '\0'; 
        
        arvore.raiz = insere(arvore.raiz, chaves_inserir[i]);
        
        if (strlen(operacao_rotacao) == 0) {
            printf("Inserir %d -> sem rotação.\n", chaves_inserir[i]);
        } else {
            printf("Inserir %d -> %s\n", chaves_inserir[i], operacao_rotacao);
        }
    }

    printf("\nArvore Completa apos insercoes (Pre-ordem): ");
    pre_ordem(arvore.raiz);
    printf("\n\n-------------------------------------------------\n\n");

    printf("2B\n");
    int chaves_remover[] = {58, 59, 66, 18};
    int num_remover = sizeof(chaves_remover) / sizeof(chaves_remover[0]);

    for (int i = 0; i < num_remover; i++) {
        operacao_rotacao[0] = '\0'; //limpa global
        
        arvore.raiz = remove_n(arvore.raiz, chaves_remover[i]);
        
        if (strlen(operacao_rotacao) == 0) {
            printf("Remoção %d -> sem rotação.\n", chaves_remover[i]);
        } else {
            printf("Remoção %d -> %s\n", chaves_remover[i], operacao_rotacao);
        }
    }

    printf("\nArvore Final apos remocoes (Pre-ordem): ");
    pre_ordem(arvore.raiz);
    printf("\n");

    return 0;
}