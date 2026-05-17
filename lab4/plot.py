# Daniel Santana Souza - 2310995
#
# Plota o grafico "numero de chaves inseridas X numero de colisoes geradas"
# a partir do CSV produzido por hash.c. Gera dois arquivos PNG:
#   - colisoes.png            : as 3 estrategias sobrepostas
#   - colisoes_duplo_hash.png : apenas a estrategia escolhida (duplo hash)

import csv
import os
import sys

import matplotlib
matplotlib.use("Agg")            # backend sem display
import matplotlib.pyplot as plt


CSV_PATH = os.path.join(os.path.dirname(__file__), "colisoes.csv")
TAMANHO_TABELA = 4919


def carrega_csv(caminho):
    n_chaves, linear, quadratica, duplo = [], [], [], []
    with open(caminho, newline="") as f:
        leitor = csv.DictReader(f)
        for linha in leitor:
            n_chaves.append(int(linha["n_chaves"]))
            linear.append(int(linha["linear"]))
            quadratica.append(int(linha["quadratica"]))
            duplo.append(int(linha["duplo_hash"]))
    return n_chaves, linear, quadratica, duplo


def plota_comparativo(n_chaves, linear, quadratica, duplo, destino):
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(n_chaves, linear,     marker="o", linewidth=1.5,
            label=f"Sondagem linear (total={linear[-1]})")
    ax.plot(n_chaves, quadratica, marker="s", linewidth=1.5,
            label=f"Sondagem quadratica (total={quadratica[-1]})")
    ax.plot(n_chaves, duplo,      marker="^", linewidth=1.5,
            label=f"Duplo hash (total={duplo[-1]})")

    ax.set_title(
        f"Colisoes X chaves inseridas em uma tabela hash (M = {TAMANHO_TABELA})"
    )
    ax.set_xlabel("Numero de chaves inseridas")
    ax.set_ylabel("Numero acumulado de colisoes")
    ax.grid(True, linestyle="--", alpha=0.5)
    ax.legend(loc="upper left")

    # marca a transicao do fator de carga ~0.5 e ~0.8
    for alpha, cor in [(0.5, "tab:gray"), (0.8, "tab:red")]:
        x = int(alpha * TAMANHO_TABELA)
        if x <= max(n_chaves):
            ax.axvline(x, linestyle=":", color=cor, alpha=0.6,
                       label=f"fator de carga = {alpha:.1f}")
    ax.legend(loc="upper left")

    fig.tight_layout()
    fig.savefig(destino, dpi=150)
    plt.close(fig)


def plota_individual(n_chaves, valores, titulo, destino):
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(n_chaves, valores, marker="o", color="tab:green", linewidth=1.8)
    ax.set_title(titulo)
    ax.set_xlabel("Numero de chaves inseridas")
    ax.set_ylabel("Numero acumulado de colisoes")
    ax.grid(True, linestyle="--", alpha=0.5)
    fig.tight_layout()
    fig.savefig(destino, dpi=150)
    plt.close(fig)


def main():
    if not os.path.exists(CSV_PATH):
        print(f"Erro: arquivo {CSV_PATH} nao encontrado. "
              "Compile e execute 'hash.c' antes.", file=sys.stderr)
        sys.exit(1)

    n_chaves, linear, quadratica, duplo = carrega_csv(CSV_PATH)

    out_dir = os.path.dirname(__file__)
    comp = os.path.join(out_dir, "colisoes.png")
    indiv = os.path.join(out_dir, "colisoes_duplo_hash.png")

    plota_comparativo(n_chaves, linear, quadratica, duplo, comp)
    plota_individual(
        n_chaves, duplo,
        f"Duplo hash (M = {TAMANHO_TABELA}) - estrategia escolhida",
        indiv,
    )

    print(f"Gerado: {comp}")
    print(f"Gerado: {indiv}")


if __name__ == "__main__":
    main()
