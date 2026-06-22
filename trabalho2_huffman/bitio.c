// Daniel Santana Souza - 2310995
// Trabalho 2 - Compactador / Descompactador de Huffman
//
// Implementacao do modulo de leitura/escrita de bits em arquivo.

#include "bitio.h"

void bw_init(t_bitwriter *bw, FILE *f)
{
    bw->f = f;
    bw->buf = 0;
    bw->nbits = 0;
}

void bw_write_bit(t_bitwriter *bw, int bit)
{
    bw->buf = (uint8_t) ((bw->buf << 1) | (bit & 1));
    bw->nbits++;
    if (bw->nbits == 8)
    {
        fputc(bw->buf, bw->f);
        bw->buf = 0;
        bw->nbits = 0;
    }
}

void bw_write_bits(t_bitwriter *bw, const char *bits)
{
    while (*bits != '\0')
    {
        bw_write_bit(bw, (*bits == '1') ? 1 : 0);
        bits++;
    }
}

void bw_flush(t_bitwriter *bw)
{
    if (bw->nbits > 0)
    {
        bw->buf = (uint8_t) (bw->buf << (8 - bw->nbits));
        fputc(bw->buf, bw->f);
        bw->buf = 0;
        bw->nbits = 0;
    }
}

void br_init(t_bitreader *br, FILE *f)
{
    br->f = f;
    br->buf = 0;
    br->nbits = 0;
    br->eof = 0;
}

int br_read_bit(t_bitreader *br)
{
    int bit;

    if (br->nbits == 0)
    {
        int c = fgetc(br->f);
        if (c == EOF)
        {
            br->eof = 1;
            return -1;
        }
        br->buf = (uint8_t) c;
        br->nbits = 8;
    }
    bit = (br->buf >> 7) & 1;
    br->buf = (uint8_t) (br->buf << 1);
    br->nbits--;
    return bit;
}
