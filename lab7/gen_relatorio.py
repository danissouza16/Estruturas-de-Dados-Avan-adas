#!/usr/bin/env python3
# Gera relatorio.pdf a partir de relatorio.md.
# Uso: python3 gen_relatorio.py
#
# Renderizador de Markdown minimalista (cabecalhos, paragrafos, listas e
# blocos de codigo) suficiente para este relatorio. Usa as fontes DejaVu
# (Unicode) para suportar acentuacao e a arte ASCII da arvore.

from fpdf import FPDF

FONT_DIR = "/usr/share/fonts/truetype/dejavu"
SRC = "relatorio.md"
OUT = "relatorio.pdf"


class Relatorio(FPDF):
    def header(self):
        pass

    def footer(self):
        self.set_y(-15)
        self.set_font("DejaVu", "I", 8)
        self.set_text_color(120, 120, 120)
        self.cell(0, 10, f"Pagina {self.page_no()}", align="C")
        self.set_text_color(0, 0, 0)


def main():
    pdf = Relatorio(format="A4")
    pdf.set_auto_page_break(auto=True, margin=18)
    pdf.set_margins(20, 18, 20)
    pdf.add_font("DejaVu", "", f"{FONT_DIR}/DejaVuSans.ttf")
    pdf.add_font("DejaVu", "B", f"{FONT_DIR}/DejaVuSans-Bold.ttf")
    pdf.add_font("DejaVu", "I", f"{FONT_DIR}/DejaVuSans.ttf")
    pdf.add_font("Mono", "", f"{FONT_DIR}/DejaVuSansMono.ttf")
    pdf.add_page()

    with open(SRC, encoding="utf-8") as fh:
        lines = fh.read().split("\n")

    epw = pdf.epw  # largura util
    i = 0
    while i < len(lines):
        line = lines[i]

        # bloco de codigo
        if line.strip().startswith("```"):
            code = []
            i += 1
            while i < len(lines) and not lines[i].strip().startswith("```"):
                code.append(lines[i])
                i += 1
            i += 1  # pula o fechamento
            render_code(pdf, code, epw)
            continue

        # cabecalhos
        if line.startswith("# "):
            heading(pdf, line[2:], size=17, top=2, bottom=3)
        elif line.startswith("## "):
            heading(pdf, line[3:], size=14, top=4, bottom=2)
        elif line.startswith("### "):
            heading(pdf, line[4:], size=12, top=3, bottom=1)
        elif line.strip() == "":
            pdf.ln(2)
        elif line.lstrip().startswith("- "):
            bullet(pdf, line.strip()[2:], epw)
        else:
            paragraph(pdf, line, epw)
        i += 1

    pdf.output(OUT)
    print("Gerado:", OUT)


def heading(pdf, text, size, top, bottom):
    pdf.ln(top)
    pdf.set_font("DejaVu", "B", size)
    write_inline(pdf, text, line_h=size * 0.5 + 2, base_style="B", base_size=size)
    pdf.ln(bottom)


def paragraph(pdf, text, epw):
    pdf.set_font("DejaVu", "", 11)
    write_inline(pdf, text, line_h=5.6)


def bullet(pdf, text, epw):
    pdf.set_font("DejaVu", "", 11)
    pdf.set_x(pdf.l_margin + 4)
    pdf.cell(4, 5.6, "\u2022")
    x = pdf.get_x()
    pdf.set_x(x)
    write_inline(pdf, text, line_h=5.6, indent=pdf.l_margin + 8)


def render_code(pdf, code, epw):
    pdf.ln(1)
    pdf.set_font("Mono", "", 8.5)
    pdf.set_fill_color(244, 244, 246)
    pad = 2
    for c in code:
        # quebra simples para linhas muito longas
        pdf.set_x(pdf.l_margin)
        pdf.multi_cell(epw, 4.6, c if c else " ", fill=True)
    pdf.set_fill_color(255, 255, 255)
    pdf.ln(1)


def write_inline(pdf, text, line_h, indent=None, base_style="", base_size=11):
    """Escreve texto tratando **negrito** e `codigo` inline."""
    if indent is None:
        indent = pdf.l_margin
    pdf.set_x(indent)
    pieces = []
    i = 0
    cur = ""
    bold = (base_style == "B")
    mono = False
    while i < len(text):
        if text[i:i + 2] == "**":
            pieces.append((cur, bold, mono)); cur = ""
            bold = not bold
            i += 2
        elif text[i] == "`":
            pieces.append((cur, bold, mono)); cur = ""
            mono = not mono
            i += 1
        else:
            cur += text[i]
            i += 1
    pieces.append((cur, bold, mono))

    for txt, b, m in pieces:
        if not txt:
            continue
        if m:
            pdf.set_font("Mono", "", base_size - 2 if base_size > 9 else base_size)
        else:
            pdf.set_font("DejaVu", "B" if b else "", base_size)
        pdf.write(line_h, txt)
    pdf.ln(line_h)


if __name__ == "__main__":
    main()
