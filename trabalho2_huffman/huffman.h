// Daniel Santana Souza - 2310995
// Trabalho 2 - Compactador / Descompactador de Huffman

#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <stdio.h>

#define NSIMB 256                /* alfabeto e o byte (ASCII estendido)         */
#define COD_BITS_MAX (NSIMB - 1) /* tamanho maximo possivel de um codigo Huffman */

/*
 * No da arvore de Huffman.
 *
 * Folhas guardam o simbolo (campo 'simb') e tem 'esq' e 'dir' iguais a NULL.
 * Nos internos tem 'simb' indefinido e dois filhos nao nulos.
 * O campo 'freq' guarda, em uma folha, a frequencia do simbolo no texto
 * original e, em um no interno, a soma das frequencias dos filhos.
 */
typedef struct hnode
{
    uint64_t freq;
    int simb;
    struct hnode *esq;
    struct hnode *dir;
} t_hnode;

/*
 * Tabela de codigos por simbolo.
 *
 * 'bits' guarda o codigo em binario, do bit mais significativo (raiz da arvore)
 * para o menos significativo (folha), apenas com '0' e '1' como caracteres.
 * 'tam'  guarda o numero de bits do codigo (0 se o simbolo nao aparece no
 *        arquivo de entrada).
 */
typedef struct
{
    char bits[COD_BITS_MAX + 1];
    int tam;
} t_codigo;

/* Constroi a arvore de Huffman a partir de um vetor de frequencias de
 * tamanho NSIMB (indexado pelo byte). Devolve a raiz da arvore ou NULL se
 * nao houver nenhum simbolo com frequencia maior que zero. Se houver um
 * unico simbolo com frequencia positiva no arquivo, a funcao cria mesmo
 * assim uma arvore com dois nos para que esse simbolo receba um codigo
 * de pelo menos 1 bit. */
t_hnode *huffman_constroi(const uint64_t freq[NSIMB]);

/* Libera recursivamente todos os nos da arvore. */
void huffman_libera(t_hnode *arv);

/* Preenche o vetor 'tab' (tamanho NSIMB) com o codigo binario de cada
 * simbolo presente na arvore. Simbolos ausentes ficam com tam = 0. */
void huffman_gera_tabela(t_hnode *arv, t_codigo tab[NSIMB]);

/* Imprime no arquivo 'out' (use stdout para a tela) a tabela de
 * frequencias e a tabela de codigos lado a lado, no formato pedido no
 * enunciado. So as linhas com frequencia > 0 sao impressas. */
void huffman_imprime_tabela(const uint64_t freq[NSIMB],
                            const t_codigo tab[NSIMB],
                            FILE *out);

/*
 * Cabecalho do arquivo compactado.
 *
 * Formato (little-endian):
 *
 *   bytes  0..3 : assinatura ASCII "HUFF"
 *   bytes  4..7 : total de simbolos no arquivo original (uint32)
 *   bytes  8..9 : numero de simbolos distintos N (uint16; 0 significa 256)
 *   em seguida, N entradas, cada uma com:
 *       1 byte  : codigo do simbolo
 *       8 bytes : frequencia (uint64)
 *   em seguida, o fluxo de bits dos codigos de Huffman, ate o fim do arquivo.
 */
#define HUFF_MAGIC "HUFF"
#define HUFF_MAGIC_LEN 4

/* Escreve o cabecalho no arquivo (deve estar aberto em "wb"). */
void huffman_escreve_cabecalho(FILE *f,
                               const uint64_t freq[NSIMB],
                               uint32_t total);

/* Le o cabecalho do arquivo (deve estar aberto em "rb") e preenche
 * 'freq' (frequencias por simbolo, zera os ausentes) e '*total' (numero
 * total de simbolos no arquivo original). Devolve 1 em sucesso ou 0 em
 * caso de cabecalho invalido. */
int huffman_le_cabecalho(FILE *f, uint64_t freq[NSIMB], uint32_t *total);

#endif /* HUFFMAN_H */
