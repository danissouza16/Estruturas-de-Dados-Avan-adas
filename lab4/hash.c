// Daniel Santana Souza - 2310995
//
// Lab 4 - Tabelas de dispersão (hash) com endereçamento aberto.
//
// O arquivo "CPFsValidos.txt" contém 4096 CPFs (números de 11 dígitos,
// possivelmente com zeros à esquerda). O programa carrega esses CPFs,
// converte cada um em um unsigned long long e os insere, um a um, em
// uma tabela hash de tamanho M = 4919 (primo, com ~20% de folga sobre
// 4096, não próximo a uma potência de 2 e congruente a 3 módulo 4).
//
// Três estratégias de tratamento de colisões via endereçamento aberto
// são executadas e comparadas:
//   1) Sondagem linear     : h(x,k) = (h1(x) + k) mod M
//   2) Sondagem quadrática : h(x,k) = (h1(x) + k + k*k) mod M
//   3) Duplo hash          : h(x,k) = (h1(x) + k * h2(x)) mod M
//      com h2(x) = 1 + (x mod (M-1))
//
// A função hash primária é a clássica do "método da divisão" recomendada
// nos slides da disciplina:
//      h1(x) = x mod M
//
// Para cada estratégia o programa registra, em pontos de checkpoint
// (100, 200, 300, ..., 4000, 4096 chaves inseridas), o número total
// (acumulado) de colisões geradas até o momento. Aqui contamos como
// "colisão" cada tentativa de sondagem que encontrou a posição já
// ocupada (ou seja: número de comparações além da primeira para cada
// inserção). Os resultados são escritos em "colisoes.csv" para serem
// plotados em um gráfico.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M           4919      /* tamanho da tabela: primo, ~20% de folga
                                 sobre 4096, M mod 4 == 3 */
#define MAX_CPFS    4200      /* capacidade do vetor de leitura */
#define PASSO        100      /* passo dos checkpoints do gráfico */
#define VAZIO          0ULL   /* CPFs válidos nunca são 0 */

typedef unsigned long long u64;

typedef struct {
    u64 chaves[M];
} Tabela;

/* ------------------------------------------------------------------ */
/* Funções de dispersão                                               */
/* ------------------------------------------------------------------ */

static unsigned int h1(u64 x) {
    return (unsigned int)(x % (u64)M);
}

static unsigned int h2(u64 x) {
    /* h2 retorna valor em [1, M-1]. Como M é primo, gcd(h2, M) = 1,
       garantindo que o duplo hash visita as M posições da tabela. */
    return 1u + (unsigned int)(x % (u64)(M - 1));
}

/* ------------------------------------------------------------------ */
/* Inicialização                                                       */
/* ------------------------------------------------------------------ */

static void inicializa(Tabela *t) {
    for (int i = 0; i < M; i++) {
        t->chaves[i] = VAZIO;
    }
}

/* ------------------------------------------------------------------ */
/* Inserção                                                            */
/*                                                                     */
/* Cada função abaixo devolve, em *colisoes, o número de posições      */
/* ocupadas visitadas até a inserção bem-sucedida (k tentativas que    */
/* falharam antes de achar slot vazio). Retorna:                       */
/*    1  -> chave inserida                                             */
/*    0  -> chave já presente (não conta como inserção)                */
/*   -1  -> falhou em achar slot (apenas teoricamente possível na      */
/*          sondagem quadrática)                                       */
/* ------------------------------------------------------------------ */

static int insere_linear(Tabela *t, u64 chave, int *colisoes) {
    unsigned int idx0 = h1(chave);
    for (unsigned int k = 0; k < (unsigned int)M; k++) {
        unsigned int idx = (idx0 + k) % (unsigned int)M;
        if (t->chaves[idx] == VAZIO) {
            t->chaves[idx] = chave;
            *colisoes = (int)k;
            return 1;
        }
        if (t->chaves[idx] == chave) {
            *colisoes = (int)k;
            return 0;
        }
    }
    *colisoes = M;
    return -1;
}

static int insere_quadratica(Tabela *t, u64 chave, int *colisoes) {
    unsigned int idx0 = h1(chave);
    for (unsigned int k = 0; k < (unsigned int)M; k++) {
        /* desvio = c1*k + c2*k^2, com c1 = c2 = 1 */
        unsigned int desvio = (k + k * k) % (unsigned int)M;
        unsigned int idx    = (idx0 + desvio) % (unsigned int)M;
        if (t->chaves[idx] == VAZIO) {
            t->chaves[idx] = chave;
            *colisoes = (int)k;
            return 1;
        }
        if (t->chaves[idx] == chave) {
            *colisoes = (int)k;
            return 0;
        }
    }
    *colisoes = M;
    return -1;
}

static int insere_duplo(Tabela *t, u64 chave, int *colisoes) {
    unsigned int idx0   = h1(chave);
    unsigned int passo  = h2(chave);
    for (unsigned int k = 0; k < (unsigned int)M; k++) {
        unsigned int idx = (idx0 + k * passo) % (unsigned int)M;
        if (t->chaves[idx] == VAZIO) {
            t->chaves[idx] = chave;
            *colisoes = (int)k;
            return 1;
        }
        if (t->chaves[idx] == chave) {
            *colisoes = (int)k;
            return 0;
        }
    }
    *colisoes = M;
    return -1;
}

/* ------------------------------------------------------------------ */
/* Leitura do arquivo                                                  */
/* ------------------------------------------------------------------ */

static int carrega_cpfs(const char *nome, u64 *vetor, int max) {
    FILE *arq = fopen(nome, "r");
    if (arq == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir %s\n", nome);
        exit(1);
    }

    int n = 0;
    char linha[32];
    while (fgets(linha, sizeof(linha), arq) != NULL) {
        /* remove eventual quebra de linha / espaços */
        size_t len = strlen(linha);
        while (len > 0 && (linha[len-1] == '\n' || linha[len-1] == '\r' ||
                           linha[len-1] == ' '  || linha[len-1] == '\t')) {
            linha[--len] = '\0';
        }
        if (len == 0) continue;

        u64 cpf = 0ULL;
        int ok = 1;
        for (size_t i = 0; i < len; i++) {
            if (linha[i] < '0' || linha[i] > '9') { ok = 0; break; }
            cpf = cpf * 10ULL + (u64)(linha[i] - '0');
        }
        if (!ok) continue;

        if (n >= max) {
            fprintf(stderr, "Erro: arquivo excede capacidade MAX_CPFS=%d\n", max);
            exit(1);
        }
        vetor[n++] = cpf;
    }
    fclose(arq);
    return n;
}

/* ------------------------------------------------------------------ */
/* Estatística por estratégia                                         */
/* ------------------------------------------------------------------ */

typedef int (*FnInserir)(Tabela *, u64, int *);

typedef struct {
    const char *nome;
    FnInserir   inserir;
    /* histórico: colisões acumuladas em cada checkpoint */
    long       *colisoes_acum;
    int         max_probes;     /* maior cluster encontrado em uma inserção */
    long        total_colisoes; /* idêntico ao último checkpoint */
} Resultado;

static void executa(Resultado *r, const u64 *cpfs, int n,
                    const int *checkpoints, int n_checkpoints) {
    Tabela t;
    inicializa(&t);

    long colisoes_acum = 0;
    int  proximo_cp    = 0;
    r->max_probes      = 0;

    for (int i = 0; i < n; i++) {
        int c = 0;
        int ret = r->inserir(&t, cpfs[i], &c);
        if (ret < 0) {
            fprintf(stderr,
                "[%s] AVISO: nao conseguiu inserir o CPF %llu apos %d "
                "tentativas (i=%d).\n",
                r->nome, cpfs[i], M, i);
        }
        colisoes_acum += c;
        if (c > r->max_probes) r->max_probes = c;

        /* registra checkpoint quando passamos do total alvo */
        while (proximo_cp < n_checkpoints &&
               (i + 1) == checkpoints[proximo_cp]) {
            r->colisoes_acum[proximo_cp] = colisoes_acum;
            proximo_cp++;
        }
    }
    /* fecha pontos restantes (não deve haver) */
    while (proximo_cp < n_checkpoints) {
        r->colisoes_acum[proximo_cp++] = colisoes_acum;
    }
    r->total_colisoes = colisoes_acum;
}

/* ------------------------------------------------------------------ */
/* Programa principal                                                  */
/* ------------------------------------------------------------------ */

int main(void) {
    u64 cpfs[MAX_CPFS];
    int n = carrega_cpfs("CPFsValidos.txt", cpfs, MAX_CPFS);

    printf("CPFs lidos: %d\n", n);
    printf("Tamanho da tabela M = %d\n", M);
    printf("Fator de carga final = %.4f (folga = %.2f%%)\n",
           (double)n / (double)M,
           100.0 * ((double)M / (double)n - 1.0));
    printf("\n");

    /* monta lista de checkpoints: 100, 200, ..., 4000, n */
    int checkpoints[MAX_CPFS / PASSO + 2];
    int n_checkpoints = 0;
    for (int v = PASSO; v <= n; v += PASSO) {
        checkpoints[n_checkpoints++] = v;
    }
    if (n_checkpoints == 0 || checkpoints[n_checkpoints-1] != n) {
        checkpoints[n_checkpoints++] = n;
    }

    /* aloca vetores de resultado */
    long colisoes_lin[64];
    long colisoes_qua[64];
    long colisoes_dup[64];

    Resultado r_lin = { "linear",     insere_linear,     colisoes_lin, 0, 0 };
    Resultado r_qua = { "quadratica", insere_quadratica, colisoes_qua, 0, 0 };
    Resultado r_dup = { "duplo-hash", insere_duplo,      colisoes_dup, 0, 0 };

    executa(&r_lin, cpfs, n, checkpoints, n_checkpoints);
    executa(&r_qua, cpfs, n, checkpoints, n_checkpoints);
    executa(&r_dup, cpfs, n, checkpoints, n_checkpoints);

    /* resumo no stdout */
    printf("Resumo (colisoes totais ao inserir os %d CPFs):\n", n);
    printf("  Sondagem linear     : %ld  (maior cluster = %d)\n",
           r_lin.total_colisoes, r_lin.max_probes);
    printf("  Sondagem quadratica : %ld  (maior cluster = %d)\n",
           r_qua.total_colisoes, r_qua.max_probes);
    printf("  Duplo hash          : %ld  (maior cluster = %d)\n",
           r_dup.total_colisoes, r_dup.max_probes);
    printf("\n");

    /* tabela detalhada no stdout */
    printf("%-10s %-12s %-12s %-12s\n",
           "n_chaves", "linear", "quadratica", "duplo_hash");
    for (int i = 0; i < n_checkpoints; i++) {
        printf("%-10d %-12ld %-12ld %-12ld\n",
               checkpoints[i],
               colisoes_lin[i], colisoes_qua[i], colisoes_dup[i]);
    }

    /* CSV para plotagem */
    FILE *csv = fopen("colisoes.csv", "w");
    if (csv == NULL) {
        fprintf(stderr, "Erro ao criar colisoes.csv\n");
        return 1;
    }
    fprintf(csv, "n_chaves,linear,quadratica,duplo_hash\n");
    for (int i = 0; i < n_checkpoints; i++) {
        fprintf(csv, "%d,%ld,%ld,%ld\n",
                checkpoints[i],
                colisoes_lin[i], colisoes_qua[i], colisoes_dup[i]);
    }
    fclose(csv);

    printf("\nArquivo 'colisoes.csv' gerado.\n");
    return 0;
}
