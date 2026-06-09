// Daniel Santana Souza - 2310995
// Laboratório 5 - Árvore B (ordem 2)
//
// Módulo: implementação das funções de construção, travessia e liberação
// da árvore B, incluindo a função intervalo() pedida no enunciado.

#include <stdio.h>
#include <stdlib.h>
#include "arvoreB.h"

t_no *cria_no(int nchaves, const int chaves[], t_no *ramos[])
{
    int i;
    t_no *no = (t_no *) malloc(sizeof(t_no));
    if (no == NULL)
    {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        exit(1);
    }

    /* ndesc = numero de chaves + 1 (vale tambem para folhas) */
    no->ndesc = nchaves + 1;

    for (i = 0; i < nchaves; i++)
        no->chave[i] = chaves[i];

    for (i = 0; i <= nchaves; i++)
        no->ramo[i] = (ramos != NULL) ? ramos[i] : NULL;

    return no;
}

t_no *cria_folha(int nchaves, const int chaves[])
{
    return cria_no(nchaves, chaves, NULL);
}

/*
 * intervalo(): percorre a arvore B em ordem simetrica (in-order) e imprime
 * as chaves x tais que lim_inf < x < lim_sup.
 *
 * A travessia simetrica de um no com k = ndesc - 1 chaves visita:
 *   ramo[0], chave[0], ramo[1], chave[1], ..., chave[k-1], ramo[k]
 *
 * Para evitar percorrer subarvores desnecessarias, sao aplicadas duas podas:
 *   - so descemos para a subarvore a esquerda de chave[i] (ramo[i]) quando
 *     chave[i] > lim_inf, pois caso contrario todas as chaves dessa
 *     subarvore sao <= chave[i] <= lim_inf e nenhuma estaria no intervalo;
 *   - assim que encontramos uma chave >= lim_sup encerramos o no, pois todas
 *     as chaves seguintes (e suas subarvores a direita) sao ainda maiores.
 */
void intervalo(t_no *arv, int lim_inf, int lim_sup)
{
    int i;
    int nchaves;

    if (arv == NULL)
        return;

    nchaves = arv->ndesc - 1;

    for (i = 0; i < nchaves; i++)
    {
        /* subarvore a esquerda da chave i */
        if (arv->chave[i] > lim_inf)
            intervalo(arv->ramo[i], lim_inf, lim_sup);

        /* a propria chave */
        if (arv->chave[i] > lim_inf && arv->chave[i] < lim_sup)
            printf("%d ", arv->chave[i]);

        /* poda: nao ha chaves validas a direita desta */
        if (arv->chave[i] >= lim_sup)
            return;
    }

    /* subarvore a direita da ultima chave */
    intervalo(arv->ramo[nchaves], lim_inf, lim_sup);
}

void em_ordem(t_no *arv)
{
    int i;
    int nchaves;

    if (arv == NULL)
        return;

    nchaves = arv->ndesc - 1;

    for (i = 0; i < nchaves; i++)
    {
        em_ordem(arv->ramo[i]);
        printf("%d ", arv->chave[i]);
    }
    em_ordem(arv->ramo[nchaves]);
}

void libera_arvore(t_no *arv)
{
    int i;

    if (arv == NULL)
        return;

    for (i = 0; i < arv->ndesc; i++)
        libera_arvore(arv->ramo[i]);

    free(arv);
}
