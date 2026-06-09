// Daniel Santana Souza - 2310995
// Laboratório 5 - Árvore B (ordem 2)
//
// Módulo principal: constrói "à mão" uma árvore B de ordem 2 (cada nó com
// até 4 chaves e 5 ponteiros) e testa a função intervalo() com os limites
// pedidos no enunciado: [5, 300], [50, 100] e [70, 135].
//
// Árvore construída (3 níveis, balanceada):
//
//                            [100 | 200]
//              .______________/    |    \______________.
//             /                    |                    \.
//        [30 | 60]            [130 | 160]                [250]
//       /    |    \           /    |    \               /    \.
//  [5,10,    [40,  [70,   [110,  [140,  [170,    [210,220,  [260,270,
//   15,20]    50]  80,90]  120]   150]  180,190]  230,240]   280,300]
//
// Travessia em ordem simétrica (todas as chaves, crescente):
//   5 10 15 20 30 40 50 60 70 80 90 100 110 120 130 140 150 160
//   170 180 190 200 210 220 230 240 250 260 270 280 300

#include <stdio.h>
#include "arvoreB.h"

/* Constrói e devolve a raiz da árvore B de exemplo. */
static t_no *constroi_arvore(void)
{
    /* ----- Folhas da subárvore esquerda (abaixo de [30|60]) ----- */
    int f0[] = {5, 10, 15, 20};
    int f1[] = {40, 50};
    int f2[] = {70, 80, 90};

    /* ----- Folhas da subárvore central (abaixo de [130|160]) ----- */
    int f3[] = {110, 120};
    int f4[] = {140, 150};
    int f5[] = {170, 180, 190};

    /* ----- Folhas da subárvore direita (abaixo de [250]) ----- */
    int f6[] = {210, 220, 230, 240};
    int f7[] = {260, 270, 280, 300};

    int ce[] = {30, 60};
    int cc[] = {130, 160};
    int cd[] = {250};
    int raiz_chaves[] = {100, 200};

    t_no *l0 = cria_folha(4, f0);
    t_no *l1 = cria_folha(2, f1);
    t_no *l2 = cria_folha(3, f2);
    t_no *l3 = cria_folha(2, f3);
    t_no *l4 = cria_folha(2, f4);
    t_no *l5 = cria_folha(3, f5);
    t_no *l6 = cria_folha(4, f6);
    t_no *l7 = cria_folha(4, f7);

    t_no *ramos_esq[] = {l0, l1, l2};
    t_no *esq = cria_no(2, ce, ramos_esq);

    t_no *ramos_cen[] = {l3, l4, l5};
    t_no *cen = cria_no(2, cc, ramos_cen);

    t_no *ramos_dir[] = {l6, l7};
    t_no *dir = cria_no(1, cd, ramos_dir);

    t_no *ramos_raiz[] = {esq, cen, dir};
    return cria_no(2, raiz_chaves, ramos_raiz);
}

/* Executa um caso de teste imprimindo o intervalo (lim_inf, lim_sup). */
static void testa_intervalo(t_no *arv, int lim_inf, int lim_sup)
{
    printf("intervalo [%d, %d] -> chaves x com %d < x < %d:\n  ",
           lim_inf, lim_sup, lim_inf, lim_sup);
    intervalo(arv, lim_inf, lim_sup);
    printf("\n\n");
}

int main(void)
{
    t_no *raiz = constroi_arvore();

    printf("Arvore B de ordem 2 (em ordem simetrica):\n  ");
    em_ordem(raiz);
    printf("\n\n");

    printf("=== Testes da funcao intervalo() ===\n\n");
    testa_intervalo(raiz, 5, 300);   /* a) */
    testa_intervalo(raiz, 50, 100);  /* b) */
    testa_intervalo(raiz, 70, 135);  /* c) */

    libera_arvore(raiz);
    return 0;
}
