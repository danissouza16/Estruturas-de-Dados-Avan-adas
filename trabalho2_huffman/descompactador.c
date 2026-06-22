// Daniel Santana Souza - 2310995
// Trabalho 2 - Compactador / Descompactador de Huffman
//
// Programa descompactador.
//
// Uso:
//   ./descompactador <arquivo_compactado> <arquivo_saida>
//
// Le um arquivo .huf gerado pelo programa compactador, reconstroi a arvore
// de Huffman a partir das frequencias guardadas no cabecalho, exibe a
// tabela e decodifica o fluxo de bits, gravando o arquivo de texto
// recuperado.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "huffman.h"
#include "bitio.h"

static int descompacta(FILE *fin, FILE *fout, t_hnode *raiz, uint32_t total)
{
    t_bitreader br;
    t_hnode *atual;
    uint32_t escritos = 0;

    br_init(&br, fin);
    atual = raiz;

    while (escritos < total)
    {
        int bit;

        /* Se a arvore so tem a raiz como folha (1 simbolo distinto), a
         * iteracao precisa ainda consumir 1 bit por simbolo. O modulo
         * huffman ja garante que neste caso a arvore tem 2 nos (uma
         * folha real e uma folha fantasma) e o codigo do simbolo e "0". */
        if (atual->esq == NULL && atual->dir == NULL)
        {
            fputc(atual->simb, fout);
            escritos++;
            atual = raiz;
            continue;
        }

        bit = br_read_bit(&br);
        if (bit < 0)
        {
            fprintf(stderr,
                    "Erro: fim de arquivo inesperado (decodificados %u de %u).\n",
                    escritos, total);
            return 0;
        }

        atual = (bit == 0) ? atual->esq : atual->dir;
        if (atual == NULL)
        {
            fprintf(stderr,
                    "Erro: codigo invalido encontrado durante a decodificacao.\n");
            return 0;
        }

        if (atual->esq == NULL && atual->dir == NULL)
        {
            /* A folha fantasma do caso de um simbolo so tem simb = -1 e
             * jamais sera atingida pelo fluxo (frequencia 0). */
            if (atual->simb < 0)
            {
                fprintf(stderr,
                        "Erro: folha invalida (codigo inconsistente).\n");
                return 0;
            }
            fputc(atual->simb, fout);
            escritos++;
            atual = raiz;
        }
    }
    return 1;
}

int main(int argc, char *argv[])
{
    FILE *fin, *fout;
    uint64_t freq[NSIMB];
    t_codigo tab[NSIMB];
    t_hnode *arv;
    uint32_t total = 0;
    long pos_inicio_dados;
    long tam_entrada;

    if (argc != 3)
    {
        fprintf(stderr,
                "Uso: %s <arquivo_compactado> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    fin = fopen(argv[1], "rb");
    if (fin == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'.\n", argv[1]);
        return 1;
    }

    if (!huffman_le_cabecalho(fin, freq, &total))
    {
        fprintf(stderr, "Erro: cabecalho invalido em '%s'.\n", argv[1]);
        fclose(fin);
        return 1;
    }
    pos_inicio_dados = ftell(fin);
    fseek(fin, 0, SEEK_END);
    tam_entrada = ftell(fin);
    fseek(fin, pos_inicio_dados, SEEK_SET);

    arv = huffman_constroi(freq);
    huffman_gera_tabela(arv, tab);

    printf("=== Descompactador de Huffman ===\n");
    printf("Arquivo compactado : %s (%ld bytes)\n", argv[1], tam_entrada);
    printf("Arquivo de saida   : %s\n", argv[2]);
    printf("Total de caracteres a recuperar: %u\n\n", total);

    printf("Tabela de frequencias e codigos (recuperada do cabecalho):\n\n");
    huffman_imprime_tabela(freq, tab, stdout);

    fout = fopen(argv[2], "wb");
    if (fout == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s' para escrita.\n",
                argv[2]);
        fclose(fin);
        huffman_libera(arv);
        return 1;
    }

    if (total > 0 && !descompacta(fin, fout, arv, total))
    {
        fclose(fin);
        fclose(fout);
        huffman_libera(arv);
        return 1;
    }

    fclose(fin);
    fclose(fout);

    printf("\nArquivo decodificado com sucesso (%u caracteres).\n", total);

    huffman_libera(arv);
    return 0;
}
