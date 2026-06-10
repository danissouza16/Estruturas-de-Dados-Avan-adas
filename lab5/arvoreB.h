// Daniel Santana Souza - 2310995
// Laboratório 4 - Árvore B (ordem 2)

#ifndef ARVOREB_H
#define ARVOREB_H

#define MAX 4 /* número máximo de chaves por nó (árvore de ordem 2) */
#define MIN 2 /* número mínimo de chaves por nó (exceto a raiz)      */

typedef struct no t_no;

/*
 * Estrutura de um nó da árvore B de ordem 2 (conforme o enunciado).
 *
 *   ndesc  -> número de descendentes (ponteiros) do nó. Em um nó com k
 *             chaves, ndesc é igual a k + 1. Em uma folha os ponteiros
 *             apontam para NULL, mas ndesc continua sendo k + 1.
 *   chave  -> vetor ordenado de chaves do nó (não inclui overflow).
 *   ramo   -> vetor de ponteiros para as subárvores. ramo[i] aponta para
 *             a subárvore cujas chaves são menores que chave[i].
 */
struct no
{
    int ndesc;
    int chave[MAX];
    t_no *ramo[MAX + 1];
};

/* Cria um nó com as chaves informadas e os respectivos ramos.
 * 'nchaves' é o número de chaves; 'chaves' é o vetor ordenado de chaves;
 * 'ramos' é o vetor de (nchaves + 1) ponteiros (use NULL para folhas). */
t_no *cria_no(int nchaves, const int chaves[], t_no *ramos[]);

/* Cria uma folha (nó sem filhos) a partir de um vetor de chaves. */
t_no *cria_folha(int nchaves, const int chaves[]);

/* Percorre a árvore B em ordem simétrica e imprime todas as chaves x
 * tais que lim_inf < x < lim_sup. Assume lim_inf <= lim_sup. */
void intervalo(t_no *arv, int lim_inf, int lim_sup);

/* Imprime todas as chaves da árvore em ordem simétrica (crescente). */
void em_ordem(t_no *arv);

/* Libera toda a memória alocada para a árvore. */
void libera_arvore(t_no *arv);

#endif /* ARVOREB_H */
