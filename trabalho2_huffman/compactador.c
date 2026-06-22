// Daniel Santana Souza - 2310995
// Trabalho 2 - Compactador / Descompactador de Huffman
//
// Programa compactador.
//
// Uso:
//   ./compactador <arquivo_entrada> <arquivo_saida>
//
// O programa le um arquivo texto, conta as frequencias dos bytes, constroi
// a arvore de Huffman, exibe a tabela com frequencias e codigos e grava
// o arquivo compactado seguindo o formato definido em huffman.h.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "huffman.h"
#include "bitio.h"

static int conta_frequencias(const char *caminho, uint64_t freq[NSIMB],
                             uint32_t *total)
{
    FILE *f = fopen(caminho, "rb");
    int c;
    uint64_t n = 0;

    if (f == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s' para leitura.\n",
                caminho);
        return 0;
    }

    memset(freq, 0, sizeof(uint64_t) * NSIMB);
    while ((c = fgetc(f)) != EOF)
    {
        freq[c & 0xFF]++;
        n++;
    }
    fclose(f);

    if (n > 0xFFFFFFFFULL)
    {
        fprintf(stderr,
                "Erro: arquivo grande demais (%llu bytes); limite de 2^32-1.\n",
                (unsigned long long) n);
        return 0;
    }
    *total = (uint32_t) n;
    return 1;
}

static int compacta(const char *entrada, const char *saida,
                    const uint64_t freq[NSIMB], const t_codigo tab[NSIMB])
{
    FILE *fin = fopen(entrada, "rb");
    FILE *fout;
    t_bitwriter bw;
    int c;
    uint32_t total = 0;
    uint32_t i;

    if (fin == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel reabrir '%s'.\n", entrada);
        return 0;
    }
    fout = fopen(saida, "wb");
    if (fout == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s' para escrita.\n",
                saida);
        fclose(fin);
        return 0;
    }

    for (i = 0; i < NSIMB; i++)
        total += (uint32_t) freq[i];

    huffman_escreve_cabecalho(fout, freq, total);

    bw_init(&bw, fout);
    while ((c = fgetc(fin)) != EOF)
        bw_write_bits(&bw, tab[c & 0xFF].bits);
    bw_flush(&bw);

    fclose(fin);
    fclose(fout);
    return 1;
}

/* Calcula em bits o tamanho original e o tamanho dos codigos. */
static void calcula_tamanhos(const uint64_t freq[NSIMB],
                             const t_codigo tab[NSIMB],
                             uint64_t *bits_original,
                             uint64_t *bits_codificados)
{
    int i;
    uint64_t bo = 0, bc = 0;
    for (i = 0; i < NSIMB; i++)
    {
        if (freq[i] > 0)
        {
            bo += freq[i] * 8;
            bc += freq[i] * (uint64_t) tab[i].tam;
        }
    }
    *bits_original = bo;
    *bits_codificados = bc;
}

int main(int argc, char *argv[])
{
    uint64_t freq[NSIMB];
    t_codigo tab[NSIMB];
    t_hnode *arv;
    uint32_t total = 0;
    uint64_t bits_orig, bits_cod;
    FILE *fcheck;
    long tam_saida;

    if (argc != 3)
    {
        fprintf(stderr,
                "Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    if (!conta_frequencias(argv[1], freq, &total))
        return 1;

    if (total == 0)
    {
        fprintf(stderr, "Aviso: arquivo de entrada vazio.\n");
        FILE *fout = fopen(argv[2], "wb");
        if (fout == NULL)
        {
            fprintf(stderr, "Erro ao abrir arquivo de saida.\n");
            return 1;
        }
        huffman_escreve_cabecalho(fout, freq, 0);
        fclose(fout);
        return 0;
    }

    arv = huffman_constroi(freq);
    if (arv == NULL)
    {
        fprintf(stderr, "Erro: falha ao construir a arvore de Huffman.\n");
        return 1;
    }
    huffman_gera_tabela(arv, tab);

    printf("=== Compactador de Huffman ===\n");
    printf("Arquivo de entrada : %s\n", argv[1]);
    printf("Arquivo de saida   : %s\n", argv[2]);
    printf("Total de caracteres no arquivo original: %u\n\n", total);

    printf("Tabela de frequencias e codigos:\n\n");
    huffman_imprime_tabela(freq, tab, stdout);

    if (!compacta(argv[1], argv[2], freq, tab))
    {
        huffman_libera(arv);
        return 1;
    }

    calcula_tamanhos(freq, tab, &bits_orig, &bits_cod);

    fcheck = fopen(argv[2], "rb");
    fseek(fcheck, 0, SEEK_END);
    tam_saida = ftell(fcheck);
    fclose(fcheck);

    printf("\nResumo:\n");
    printf("  Bits originais (8 por caractere): %llu bits (%llu bytes)\n",
           (unsigned long long) bits_orig,
           (unsigned long long) (bits_orig / 8));
    printf("  Bits codificados (sem cabecalho): %llu bits (%llu bytes uteis,\n"
           "                                    arredondado para %llu byte(s))\n",
           (unsigned long long) bits_cod,
           (unsigned long long) (bits_cod / 8),
           (unsigned long long) ((bits_cod + 7) / 8));
    printf("  Taxa de compressao (so dados)   : %.2f%%\n",
           bits_orig == 0 ? 0.0
                          : 100.0 * (1.0 - (double) bits_cod / (double) bits_orig));
    printf("  Tamanho final do arquivo .huf   : %ld bytes (com cabecalho)\n",
           tam_saida);

    huffman_libera(arv);
    return 0;
}
