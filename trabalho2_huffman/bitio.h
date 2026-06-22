// Daniel Santana Souza - 2310995
// Trabalho 2 - Compactador / Descompactador de Huffman
//
// Modulo de escrita/leitura de bits em arquivo. O sistema operacional
// (e a API stdio do C) trabalha em bytes; este modulo "empacota" um
// fluxo de bits em bytes na hora de gravar e desempacota na hora de ler.
//
// Convencao adotada: o primeiro bit gravado vai para o BIT MAIS
// SIGNIFICATIVO do primeiro byte (bit 7), o segundo bit vai para o bit 6
// e assim sucessivamente. A leitura segue a mesma ordem. Esta convencao
// permite escrever um codigo Huffman simplesmente percorrendo seus bits
// da esquerda para a direita.

#ifndef BITIO_H
#define BITIO_H

#include <stdint.h>
#include <stdio.h>

typedef struct
{
    FILE *f;
    uint8_t buf;   /* byte parcialmente preenchido        */
    int nbits;     /* quantos bits ja ocupados em 'buf'   */
} t_bitwriter;

typedef struct
{
    FILE *f;
    uint8_t buf;   /* byte atual sendo consumido          */
    int nbits;     /* quantos bits ainda restam em 'buf'  */
    int eof;       /* 1 se ja se atingiu o fim do arquivo */
} t_bitreader;

/* --- escrita --- */

void bw_init(t_bitwriter *bw, FILE *f);

/* Escreve um unico bit (0 ou 1). */
void bw_write_bit(t_bitwriter *bw, int bit);

/* Escreve uma sequencia de bits dada como string com '0'/'1'. */
void bw_write_bits(t_bitwriter *bw, const char *bits);

/* Descarrega o byte parcial (se houver), completando com zeros. */
void bw_flush(t_bitwriter *bw);

/* --- leitura --- */

void br_init(t_bitreader *br, FILE *f);

/* Devolve o proximo bit (0 ou 1), ou -1 se nao houver mais bits.
 * "Nao ha mais bits" significa que o arquivo acabou; bits de padding
 * de um byte final incompleto sao indistinguiveis de bits reais e
 * por isso o programa principal precisa parar de chamar esta funcao
 * com base no numero conhecido de simbolos do arquivo original. */
int br_read_bit(t_bitreader *br);

#endif /* BITIO_H */
