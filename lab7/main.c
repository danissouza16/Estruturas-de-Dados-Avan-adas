// Daniel Santana Souza - 2310995
// Laboratório 7 - Grafos: Sistema de Evacuação Inteligente
//
// Programa principal. Monta o mapa do edifício como um grafo não dirigido
// em listas de adjacência e executa os experimentos pedidos no enunciado:
//   1) representação do grafo;
//   2) DFS a partir da Recepção;
//   3) BFS para os caminhos mínimos até a Saída de Emergência;
//   4) remoção da conexão Auditorio--CorredorB e análise das componentes
//      conexas resultantes.
//
// Mapa do edifício:
//
//        +---------------+
//        | Laboratorio 1 |
//        +---------------+
//                |
//   +-----------+   +------------+   +---------------+
//   | Recepcao  |---| Corredor A |---| Laboratorio 2 |
//   +-----------+   +------------+   +---------------+
//         |
//   +-----------+   +------------+   +----------+
//   | Auditorio |---| Corredor B |---|  Escada  |
//   +-----------+   +------------+   +----------+
//                         |
//                 +---------------------+
//                 | Saida de Emergencia |
//                 +---------------------+

#include <stdio.h>

#include "grafo.h"

/* Nomes dos ambientes (sem acentos para manter a saída portável no
 * terminal). São usados de forma consistente em todo o programa. */
#define LAB1   "Laboratorio1"
#define LAB2   "Laboratorio2"
#define RECEP  "Recepcao"
#define CORRA  "CorredorA"
#define CORRB  "CorredorB"
#define AUDIT  "Auditorio"
#define ESCADA "Escada"
#define SAIDA  "Saida"

/* Constrói o grafo do edifício conforme o mapa do enunciado. */
static Grafo *constroi_mapa(void)
{
    Grafo *g = cria_grafo();
    adiciona_aresta(g, LAB1, RECEP);
    adiciona_aresta(g, RECEP, CORRA);
    adiciona_aresta(g, CORRA, LAB2);
    adiciona_aresta(g, RECEP, AUDIT);
    adiciona_aresta(g, AUDIT, CORRB);
    adiciona_aresta(g, CORRB, ESCADA);
    adiciona_aresta(g, CORRB, SAIDA);
    return g;
}

static void titulo(const char *texto)
{
    printf("\n========================================================\n");
    printf("%s\n", texto);
    printf("========================================================\n");
}

int main(void)
{
    Grafo *g = constroi_mapa();

    /* ---- 1) Representação do grafo em listas de adjacência ---- */
    titulo("1) Representacao do mapa (listas de adjacencia)");
    imprime_grafo(g);

    /* ---- 2) DFS a partir da Recepcao ---- */
    titulo("2) Busca em Profundidade (DFS) a partir da Recepcao");
    int visitados = dfs(g, RECEP);
    printf("Total de ambientes visitados: %d de %d.\n", visitados, g->nv);

    /* ---- 3) BFS: caminhos minimos ate a Saida ---- */
    titulo("3) Busca em Largura (BFS): caminhos minimos ate a Saida");
    int d1 = bfs_caminho(g, LAB1, SAIDA);
    int d2 = bfs_caminho(g, LAB2, SAIDA);
    printf("\nResumo dos deslocamentos minimos:\n");
    printf("  Laboratorio 1 -> Saida: %d deslocamentos\n", d1);
    printf("  Laboratorio 2 -> Saida: %d deslocamentos\n", d2);

    /* ---- 4) Remocao da conexao Auditorio--CorredorB ---- */
    titulo("4) Incendio: remocao da conexao Auditorio--CorredorB");
    int ok = remove_aresta(g, AUDIT, CORRB);
    printf("Conexao (Auditorio, CorredorB) removida: %s\n",
           ok ? "sim" : "nao encontrada");

    printf("\nMapa apos a remocao:\n");
    imprime_grafo(g);

    printf("\n4.a) Existe caminho entre Recepcao e Saida?\n");
    int dr = bfs_caminho(g, RECEP, SAIDA);
    printf("  Resposta: %s\n", dr == -1 ? "NAO ha mais caminho." : "Sim, ainda ha.");

    printf("\n4.c) DFS novamente a partir da Recepcao:\n");
    int vis2 = dfs(g, RECEP);
    printf("Ambientes alcancaveis a partir da Recepcao: %d de %d.\n",
           vis2, g->nv);
    printf("Ambientes isolados da Recepcao: %d.\n", g->nv - vis2);

    printf("\n4.e) Componentes conexas do novo grafo:\n");
    int nc = componentes_conexas(g);
    printf("Total de componentes conexas: %d.\n", nc);

    libera_grafo(g);
    return 0;
}
