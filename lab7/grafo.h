// Daniel Santana Souza - 2310995
// Laboratório 7 - Grafos: Sistema de Evacuação Inteligente

#ifndef GRAFO_H
#define GRAFO_H

#define MAX_NOME 32     /* tamanho máximo do nome de um ambiente            */
#define MAX_VERTICES 64 /* número máximo de ambientes suportados pelo grafo */

/*
 * Célula da lista de adjacência. Representa uma aresta incidente em um
 * vértice: 'noj' é o índice do vértice vizinho e 'prox' encadeia a lista.
 * (Segue o modelo "Viz" visto em aula, sem peso, pois o grafo não é
 * ponderado.)
 */
typedef struct viz Viz;
struct viz {
    int noj;
    Viz *prox;
};

/*
 * Grafo não dirigido com vértices nomeados.
 *   nv     -> número de vértices (ambientes) cadastrados.
 *   na     -> número de arestas (conexões) do grafo.
 *   nomes  -> nome de cada vértice; nomes[i] é o nome do vértice i.
 *   viz    -> viz[i] aponta para a lista de arestas que incidem no vértice i.
 */
typedef struct grafo Grafo;
struct grafo {
    int nv;
    int na;
    char nomes[MAX_VERTICES][MAX_NOME];
    Viz *viz[MAX_VERTICES];
};

/* Cria um grafo vazio (sem vértices nem arestas). */
Grafo *cria_grafo(void);

/* Devolve o índice do vértice de nome 'nome', ou -1 se ele não existir. */
int indice_vertice(Grafo *g, const char *nome);

/* Cadastra um novo ambiente (vértice). Se já existir, devolve o índice
 * existente; caso contrário cria e devolve o novo índice. */
int adiciona_vertice(Grafo *g, const char *nome);

/* Cria a aresta não dirigida entre os ambientes 'a' e 'b' (cadastrando-os
 * caso ainda não existam). Como o grafo é não dirigido, a ligação é
 * registrada nas duas listas de adjacência. */
void adiciona_aresta(Grafo *g, const char *a, const char *b);

/* Remove a aresta não dirigida entre 'a' e 'b' das duas listas de
 * adjacência. Devolve 1 se a aresta existia e foi removida, 0 caso
 * contrário. */
int remove_aresta(Grafo *g, const char *a, const char *b);

/* Imprime o grafo no formato de listas de adjacência. */
void imprime_grafo(Grafo *g);

/* Busca em Profundidade (DFS) a partir de 'origem'. Imprime os vértices na
 * ordem em que são visitados. Devolve o número de vértices visitados. */
int dfs(Grafo *g, const char *origem);

/* Busca em Largura (BFS) de 'origem' até 'destino'. Imprime o caminho
 * mínimo encontrado e devolve o número de deslocamentos (arestas) desse
 * caminho, ou -1 se não houver caminho. */
int bfs_caminho(Grafo *g, const char *origem, const char *destino);

/* Conta e imprime as componentes conexas do grafo. Devolve o número de
 * componentes. */
int componentes_conexas(Grafo *g);

/* Libera toda a memória alocada para o grafo. */
void libera_grafo(Grafo *g);

#endif /* GRAFO_H */
