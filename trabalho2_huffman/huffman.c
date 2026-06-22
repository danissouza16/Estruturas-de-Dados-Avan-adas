// Daniel Santana Souza - 2310995
// Trabalho 2 - Compactador / Descompactador de Huffman
//
// Modulo da arvore de Huffman.
//
// O algoritmo de Huffman gera codigos binarios de tamanho variavel onde
// os simbolos mais frequentes recebem codigos mais curtos. A propriedade
// fundamental e a de "prefixo": nenhum codigo e prefixo de outro, o que
// permite decodificar o fluxo de bits sem nenhum separador.
//
// A construcao da arvore parte de uma "floresta" de arvores triviais
// (uma folha para cada simbolo que aparece no texto, com a respectiva
// frequencia) e repetidamente combina as duas arvores de menor frequencia
// em uma nova arvore cuja raiz e um no interno com frequencia igual a
// soma das duas. Quando sobra uma unica arvore, esta e a arvore de
// Huffman.
//
// Para combinar as duas menores em cada passo usamos uma fila ordenada
// implementada como lista ligada. Como o alfabeto tem no maximo
// NSIMB = 256 simbolos, a complexidade O(n^2) e perfeitamente aceitavel
// e o codigo fica mais simples e didatico.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "huffman.h"

/* No da fila ordenada usada na construcao da arvore. */
typedef struct fnode
{
    t_hnode *arv;
    struct fnode *prox;
} t_fnode;

/* --------------------- nos da arvore --------------------- */

static t_hnode *cria_folha(int simb, uint64_t freq)
{
    t_hnode *n = (t_hnode *) malloc(sizeof(t_hnode));
    if (n == NULL)
    {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        exit(1);
    }
    n->simb = simb;
    n->freq = freq;
    n->esq = NULL;
    n->dir = NULL;
    return n;
}

static t_hnode *cria_interno(t_hnode *esq, t_hnode *dir)
{
    t_hnode *n = (t_hnode *) malloc(sizeof(t_hnode));
    if (n == NULL)
    {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        exit(1);
    }
    n->simb = -1; /* nao usado em nos internos */
    n->freq = esq->freq + dir->freq;
    n->esq = esq;
    n->dir = dir;
    return n;
}

void huffman_libera(t_hnode *arv)
{
    if (arv == NULL)
        return;
    huffman_libera(arv->esq);
    huffman_libera(arv->dir);
    free(arv);
}

/* --------------------- fila ordenada --------------------- */

/* Insere 'arv' na fila ordenada (crescente por frequencia). Em caso de
 * empate de frequencia, mantem a ordem de chegada (estavel). */
static t_fnode *fila_insere(t_fnode *fila, t_hnode *arv)
{
    t_fnode *novo = (t_fnode *) malloc(sizeof(t_fnode));
    if (novo == NULL)
    {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        exit(1);
    }
    novo->arv = arv;
    novo->prox = NULL;

    if (fila == NULL || arv->freq < fila->arv->freq)
    {
        novo->prox = fila;
        return novo;
    }

    t_fnode *p = fila;
    while (p->prox != NULL && p->prox->arv->freq <= arv->freq)
        p = p->prox;
    novo->prox = p->prox;
    p->prox = novo;
    return fila;
}

/* Remove e devolve a primeira arvore da fila (a de menor frequencia).
 * Atualiza '*fila' para apontar para o novo inicio. */
static t_hnode *fila_remove(t_fnode **fila)
{
    if (*fila == NULL)
        return NULL;
    t_fnode *cab = *fila;
    t_hnode *arv = cab->arv;
    *fila = cab->prox;
    free(cab);
    return arv;
}

/* --------------------- construcao da arvore --------------------- */

t_hnode *huffman_constroi(const uint64_t freq[NSIMB])
{
    t_fnode *fila = NULL;
    int i;
    int nsimb = 0;

    for (i = 0; i < NSIMB; i++)
    {
        if (freq[i] > 0)
        {
            fila = fila_insere(fila, cria_folha(i, freq[i]));
            nsimb++;
        }
    }

    if (nsimb == 0)
        return NULL; /* arquivo vazio */

    /* Caso degenerado: um unico simbolo aparece no arquivo. Para que ele
     * receba um codigo de pelo menos 1 bit, criamos um no interno tendo
     * esta folha a esquerda e uma folha "fantasma" (frequencia 0) a
     * direita. A folha fantasma so existe na arvore reconstruida pelo
     * descompactador atraves das frequencias, entao o codigo "0" sera
     * sempre usado para o unico simbolo do arquivo. */
    if (nsimb == 1)
    {
        t_hnode *unica = fila_remove(&fila);
        t_hnode *fantasma = cria_folha(-1, 0);
        return cria_interno(unica, fantasma);
    }

    /* Combina repetidamente as duas arvores de menor frequencia. */
    while (fila != NULL && fila->prox != NULL)
    {
        t_hnode *a = fila_remove(&fila);
        t_hnode *b = fila_remove(&fila);
        fila = fila_insere(fila, cria_interno(a, b));
    }

    t_hnode *raiz = fila_remove(&fila);
    return raiz;
}

/* --------------------- tabela de codigos --------------------- */

static void preenche_codigos(t_hnode *no, t_codigo tab[NSIMB],
                             char *buf, int prof)
{
    if (no == NULL)
        return;
    if (no->esq == NULL && no->dir == NULL)
    {
        if (no->simb < 0)
            return; /* folha fantasma do caso degenerado */
        buf[prof] = '\0';
        if (prof == 0)
        {
            /* arvore com um unico no -> codigo de 1 bit */
            tab[no->simb].bits[0] = '0';
            tab[no->simb].bits[1] = '\0';
            tab[no->simb].tam = 1;
        }
        else
        {
            memcpy(tab[no->simb].bits, buf, prof + 1);
            tab[no->simb].tam = prof;
        }
        return;
    }

    buf[prof] = '0';
    preenche_codigos(no->esq, tab, buf, prof + 1);
    buf[prof] = '1';
    preenche_codigos(no->dir, tab, buf, prof + 1);
}

void huffman_gera_tabela(t_hnode *arv, t_codigo tab[NSIMB])
{
    char buf[COD_BITS_MAX + 1];
    int i;

    for (i = 0; i < NSIMB; i++)
    {
        tab[i].bits[0] = '\0';
        tab[i].tam = 0;
    }

    if (arv == NULL)
        return;

    preenche_codigos(arv, tab, buf, 0);
}

/* --------------------- impressao da tabela --------------------- */

/* Devolve um nome legivel para o simbolo: 'A', 'B', ... ou notacao
 * descritiva para os nao imprimiveis. */
static void nome_simbolo(int simb, char *out, int n)
{
    if (simb == ' ')
        snprintf(out, n, "' ' (espaco)");
    else if (simb == '\n')
        snprintf(out, n, "'\\n' (nova linha)");
    else if (simb == '\r')
        snprintf(out, n, "'\\r' (CR)");
    else if (simb == '\t')
        snprintf(out, n, "'\\t' (tab)");
    else if (isprint((unsigned char) simb))
        snprintf(out, n, "'%c'", simb);
    else
        snprintf(out, n, "0x%02X", simb);
}

/* --------------------- cabecalho do arquivo compactado --------------------- */

static void escreve_u16(FILE *f, uint16_t v)
{
    fputc((int) (v & 0xFF), f);
    fputc((int) ((v >> 8) & 0xFF), f);
}

static void escreve_u32(FILE *f, uint32_t v)
{
    int i;
    for (i = 0; i < 4; i++)
        fputc((int) ((v >> (8 * i)) & 0xFF), f);
}

static void escreve_u64(FILE *f, uint64_t v)
{
    int i;
    for (i = 0; i < 8; i++)
        fputc((int) ((v >> (8 * i)) & 0xFF), f);
}

static int le_u16(FILE *f, uint16_t *out)
{
    int b0 = fgetc(f);
    int b1 = fgetc(f);
    if (b0 == EOF || b1 == EOF)
        return 0;
    *out = (uint16_t) ((b1 << 8) | b0);
    return 1;
}

static int le_u32(FILE *f, uint32_t *out)
{
    int i, c;
    uint32_t v = 0;
    for (i = 0; i < 4; i++)
    {
        c = fgetc(f);
        if (c == EOF)
            return 0;
        v |= ((uint32_t) c) << (8 * i);
    }
    *out = v;
    return 1;
}

static int le_u64(FILE *f, uint64_t *out)
{
    int i, c;
    uint64_t v = 0;
    for (i = 0; i < 8; i++)
    {
        c = fgetc(f);
        if (c == EOF)
            return 0;
        v |= ((uint64_t) c) << (8 * i);
    }
    *out = v;
    return 1;
}

void huffman_escreve_cabecalho(FILE *f, const uint64_t freq[NSIMB],
                               uint32_t total)
{
    int i, n = 0;

    for (i = 0; i < NSIMB; i++)
        if (freq[i] > 0)
            n++;

    fwrite(HUFF_MAGIC, 1, HUFF_MAGIC_LEN, f);
    escreve_u32(f, total);
    /* n cabe em uint16: maximo 256 (= 0x0100). Convencao: 0 nao ocorre
     * pois, se total > 0, ha pelo menos 1 simbolo distinto. */
    escreve_u16(f, (uint16_t) (n == NSIMB ? 0 : n));

    for (i = 0; i < NSIMB; i++)
    {
        if (freq[i] > 0)
        {
            fputc(i, f);
            escreve_u64(f, freq[i]);
        }
    }
}

int huffman_le_cabecalho(FILE *f, uint64_t freq[NSIMB], uint32_t *total)
{
    char magic[HUFF_MAGIC_LEN];
    uint16_t n16;
    uint32_t i;
    int nsimb;

    for (i = 0; i < NSIMB; i++)
        freq[i] = 0;

    if (fread(magic, 1, HUFF_MAGIC_LEN, f) != HUFF_MAGIC_LEN)
        return 0;
    if (memcmp(magic, HUFF_MAGIC, HUFF_MAGIC_LEN) != 0)
        return 0;
    if (!le_u32(f, total))
        return 0;
    if (!le_u16(f, &n16))
        return 0;

    nsimb = (n16 == 0) ? NSIMB : n16;

    for (i = 0; i < (uint32_t) nsimb; i++)
    {
        int c = fgetc(f);
        uint64_t fr;
        if (c == EOF)
            return 0;
        if (!le_u64(f, &fr))
            return 0;
        freq[c] = fr;
    }
    return 1;
}

void huffman_imprime_tabela(const uint64_t freq[NSIMB],
                            const t_codigo tab[NSIMB], FILE *out)
{
    int i;
    char nome[24];

    fprintf(out,
            "%-18s %-10s %-6s %s\n",
            "Caractere", "Frequencia", "Bits", "Codigo");
    fprintf(out,
            "------------------ ---------- ------ "
            "----------------------------------------\n");

    for (i = 0; i < NSIMB; i++)
    {
        if (freq[i] == 0)
            continue;
        nome_simbolo(i, nome, sizeof nome);
        fprintf(out, "%-18s %-10llu %-6d %s\n",
                nome, (unsigned long long) freq[i],
                tab[i].tam, tab[i].bits);
    }
}
