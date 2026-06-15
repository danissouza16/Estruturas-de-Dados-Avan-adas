// Daniel Santana Souza - 2310995
// Laboratório 7 - Grafos: Sistema de Evacuação Inteligente
//
// Implementação do módulo de grafo não dirigido em listas de adjacência,
// junto com os algoritmos de percurso DFS e BFS e a contagem de componentes
// conexas usados pelo sistema de evacuação.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grafo.h"

Grafo *cria_grafo(void)
{
    Grafo *g = (Grafo *) malloc(sizeof(Grafo));
    if (g == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente para criar o grafo.\n");
        exit(1);
    }
    g->nv = 0;
    g->na = 0;
    for (int i = 0; i < MAX_VERTICES; i++)
        g->viz[i] = NULL;
    return g;
}

int indice_vertice(Grafo *g, const char *nome)
{
    for (int i = 0; i < g->nv; i++)
        if (strcmp(g->nomes[i], nome) == 0)
            return i;
    return -1;
}

int adiciona_vertice(Grafo *g, const char *nome)
{
    int i = indice_vertice(g, nome);
    if (i != -1)
        return i; /* já existe */

    if (g->nv >= MAX_VERTICES) {
        fprintf(stderr, "Erro: limite de vertices (%d) atingido.\n",
                MAX_VERTICES);
        exit(1);
    }
    i = g->nv;
    strncpy(g->nomes[i], nome, MAX_NOME - 1);
    g->nomes[i][MAX_NOME - 1] = '\0';
    g->viz[i] = NULL;
    g->nv++;
    return i;
}

/* Insere o índice 'destino' no início da lista de adjacência de 'origem',
 * caso ainda não exista. Devolve 1 se inseriu, 0 se já havia a ligação. */
static int insere_vizinho(Grafo *g, int origem, int destino)
{
    for (Viz *p = g->viz[origem]; p != NULL; p = p->prox)
        if (p->noj == destino)
            return 0; /* aresta já registrada */

    Viz *novo = (Viz *) malloc(sizeof(Viz));
    if (novo == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente para criar aresta.\n");
        exit(1);
    }
    novo->noj = destino;
    novo->prox = g->viz[origem];
    g->viz[origem] = novo;
    return 1;
}

void adiciona_aresta(Grafo *g, const char *a, const char *b)
{
    int ia = adiciona_vertice(g, a);
    int ib = adiciona_vertice(g, b);

    /* Grafo não dirigido: registra a ligação nos dois sentidos. */
    int novo = insere_vizinho(g, ia, ib);
    insere_vizinho(g, ib, ia);
    if (novo)
        g->na++;
}

/* Remove o índice 'destino' da lista de adjacência de 'origem'.
 * Devolve 1 se removeu, 0 se não encontrou. */
static int remove_vizinho(Grafo *g, int origem, int destino)
{
    Viz *ant = NULL;
    Viz *p = g->viz[origem];
    while (p != NULL) {
        if (p->noj == destino) {
            if (ant == NULL)
                g->viz[origem] = p->prox;
            else
                ant->prox = p->prox;
            free(p);
            return 1;
        }
        ant = p;
        p = p->prox;
    }
    return 0;
}

int remove_aresta(Grafo *g, const char *a, const char *b)
{
    int ia = indice_vertice(g, a);
    int ib = indice_vertice(g, b);
    if (ia == -1 || ib == -1)
        return 0;

    int r1 = remove_vizinho(g, ia, ib);
    int r2 = remove_vizinho(g, ib, ia);
    if (r1 || r2) {
        g->na--;
        return 1;
    }
    return 0;
}

void imprime_grafo(Grafo *g)
{
    printf("Grafo (%d ambientes, %d conexoes) - listas de adjacencia:\n",
           g->nv, g->na);
    for (int i = 0; i < g->nv; i++) {
        printf("  [%d] %-20s ->", i, g->nomes[i]);
        for (Viz *p = g->viz[i]; p != NULL; p = p->prox)
            printf(" %s", g->nomes[p->noj]);
        printf("\n");
    }
}

/* Visita recursiva da DFS. Marca o vértice atual, imprime seu nome e desce
 * para cada vizinho ainda não visitado. 'primeiro' evita imprimir um
 * separador antes do primeiro ambiente. */
static void dfs_visita(Grafo *g, int v, int visitado[], int *primeiro)
{
    visitado[v] = 1;
    if (!*primeiro)
        printf(" -> ");
    printf("%s", g->nomes[v]);
    *primeiro = 0;

    for (Viz *p = g->viz[v]; p != NULL; p = p->prox)
        if (!visitado[p->noj])
            dfs_visita(g, p->noj, visitado, primeiro);
}

int dfs(Grafo *g, const char *origem)
{
    int ini = indice_vertice(g, origem);
    if (ini == -1) {
        printf("Ambiente \"%s\" nao encontrado.\n", origem);
        return 0;
    }

    int visitado[MAX_VERTICES] = {0};
    int primeiro = 1;
    printf("DFS a partir de \"%s\":\n  ", origem);
    dfs_visita(g, ini, visitado, &primeiro);
    printf("\n");

    int total = 0;
    for (int i = 0; i < g->nv; i++)
        total += visitado[i];
    return total;
}

int bfs_caminho(Grafo *g, const char *origem, const char *destino)
{
    int ini = indice_vertice(g, origem);
    int fim = indice_vertice(g, destino);
    if (ini == -1 || fim == -1) {
        printf("Origem ou destino inexistente.\n");
        return -1;
    }

    int visitado[MAX_VERTICES] = {0};
    int anterior[MAX_VERTICES];     /* predecessor de cada vértice na BFS */
    int dist[MAX_VERTICES];         /* distância (nº de arestas) à origem */
    for (int i = 0; i < g->nv; i++)
        anterior[i] = -1;

    int fila[MAX_VERTICES];
    int inicio = 0, traseira = 0;

    visitado[ini] = 1;
    dist[ini] = 0;
    fila[traseira++] = ini;

    while (inicio < traseira) {
        int v = fila[inicio++];
        if (v == fim)
            break; /* destino alcançado: caminho mínimo já garantido */
        for (Viz *p = g->viz[v]; p != NULL; p = p->prox) {
            if (!visitado[p->noj]) {
                visitado[p->noj] = 1;
                anterior[p->noj] = v;
                dist[p->noj] = dist[v] + 1;
                fila[traseira++] = p->noj;
            }
        }
    }

    if (!visitado[fim]) {
        printf("Nao ha caminho entre \"%s\" e \"%s\".\n", origem, destino);
        return -1;
    }

    /* Reconstrói o caminho seguindo os predecessores de trás para frente. */
    int caminho[MAX_VERTICES];
    int n = 0;
    for (int v = fim; v != -1; v = anterior[v])
        caminho[n++] = v;

    printf("Caminho minimo de \"%s\" ate \"%s\" (%d deslocamentos):\n  ",
           origem, destino, dist[fim]);
    for (int i = n - 1; i >= 0; i--) {
        printf("%s", g->nomes[caminho[i]]);
        if (i > 0)
            printf(" -> ");
    }
    printf("\n");
    return dist[fim];
}

/* Marca, por BFS iterativa, todos os vértices alcançáveis a partir de 'v'
 * (usados na contagem de componentes conexas). */
static void marca_componente(Grafo *g, int v, int visitado[])
{
    int fila[MAX_VERTICES];
    int inicio = 0, traseira = 0;
    visitado[v] = 1;
    fila[traseira++] = v;
    while (inicio < traseira) {
        int u = fila[inicio++];
        for (Viz *p = g->viz[u]; p != NULL; p = p->prox)
            if (!visitado[p->noj]) {
                visitado[p->noj] = 1;
                fila[traseira++] = p->noj;
            }
    }
}

int componentes_conexas(Grafo *g)
{
    int visitado[MAX_VERTICES] = {0};
    int n = 0;

    for (int i = 0; i < g->nv; i++) {
        if (!visitado[i]) {
            n++;
            /* Coleta os vértices desta componente para imprimi-los. */
            int antes[MAX_VERTICES];
            for (int k = 0; k < g->nv; k++)
                antes[k] = visitado[k];

            marca_componente(g, i, visitado);

            printf("  Componente %d:", n);
            for (int k = 0; k < g->nv; k++)
                if (visitado[k] && !antes[k])
                    printf(" %s", g->nomes[k]);
            printf("\n");
        }
    }
    return n;
}

void libera_grafo(Grafo *g)
{
    for (int i = 0; i < g->nv; i++) {
        Viz *p = g->viz[i];
        while (p != NULL) {
            Viz *prox = p->prox;
            free(p);
            p = prox;
        }
    }
    free(g);
}
