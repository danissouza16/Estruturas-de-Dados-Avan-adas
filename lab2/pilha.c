// Caio Nabuco Vogel - 2310878
// Daniel Santana Souza - 2310995

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct elemento {
    int info;
    struct elemento *prox;
};typedef struct elemento Elemento;


struct pilha {
    Elemento* topo; /* aponta para o topo da pilha */
};typedef struct pilha Pilha;

Pilha* pilha_cria (void);
void pilha_push(Pilha* p, int v);
int pilha_pop (Pilha* p);
void pilha_libera (Pilha* p);
int pilha_vazia (Pilha* p);
void percorre_pilha(Pilha* p);
void imprime_rec(Elemento* lst);

Elemento* primeiro; //variavel global para a comparação na parte da leitura

int main(void)
{
    Pilha* p;
    p = pilha_cria();
    FILE* arq;
    char linha[100];
    int cont =1;

    arq = fopen( "entrada_pilha.txt" , "r");
    if(arq==NULL) 
        exit(1);
    


    while(fgets(linha,sizeof linha,arq) != NULL) {
        printf("\nInicio da pilha %d\n", cont);
        for(int i = 0; i< strlen(linha); i++)
        {
            if(linha[i] == 'e')
            {
                int j = i + 1;
                char val[100]; 
                int tam;
                while(linha[j+1] != ' ' && linha[j+1] != '\0' && linha[j+1] != '\n')
                {                    
                    j++;
                }
                for(tam = 0;tam <= (j-i-1) ;tam++)
                {
                    val[tam]  = linha[i+tam+1];                
                }
                val[tam] = '\0';
                pilha_push(p, atoi(val));
                percorre_pilha(p);
            }
            if(linha[i] == 'r')
            {  
                pilha_pop(p);
                percorre_pilha(p);
            }
            
        }
        pilha_libera(p);
        p = pilha_cria();
        printf("Fim da pilha %d \n", cont);
        cont++;
    }
    pilha_libera(p);
    fclose(arq);
    return 0;

}

void imprime_rec(Elemento* lst)
{
    if (lst == NULL)
        return;
    imprime_rec(lst->prox);
    if(lst == primeiro)
    {
        printf("%d", lst->info);
    }
    else{
        printf("%d, ", lst->info);
    }
    
}

void percorre_pilha(Pilha* p)
{
    printf("[");
    primeiro = p->topo;
    imprime_rec(p->topo);
    printf("]\n");
}


Pilha* pilha_cria (void)
{
    Pilha* p = (Pilha*) malloc(sizeof(Pilha));
    if (p==NULL) exit(1);
        p->topo = NULL;
    return p;
}

void pilha_push (Pilha* p, int v)
{
    Elemento* n = (Elemento*) malloc(sizeof(Elemento));
    if (n==NULL){ /* memória esgotada */
        printf("Sem memória para alocar elemento.\n");
        exit(1); /* aborta programa */
    }
    /* insere elemento na próxima posição livre */
    n->info = v;
    n->prox = p->topo;
    p->topo = n;
}

int pilha_pop (Pilha* p)
{
    Elemento* t;
    int v;
    if (pilha_vazia(p))
    {
        printf("Pilha vazia.\n");
        exit(1); /* aborta programa */
    }
    /* retira elemento do topo */
    t = p->topo;
    v = t->info;
    p-> topo = t->prox;
    free(t);
    return v;
}

void pilha_libera (Pilha* p)
{
    Elemento *t, *q = p->topo;
    while (q!=NULL)
    {
        t = q->prox;
        free(q);
        q = t;
    }
    free(p);
}

int pilha_vazia (Pilha* p)
{
    if(p->topo == NULL)
        return 1;
    return 0;
}