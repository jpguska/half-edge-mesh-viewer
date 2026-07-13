# docs/

Capturas de tela e material visual.

**Esta pasta está vazia — e ela é a parte mais importante que falta.**

Este é um projeto gráfico: quem abre o repositório quer *ver* o resultado antes de ler qualquer linha de código. Um README de projeto visual sem imagem desperdiça a maior parte do seu impacto.

## O que capturar

Sugestão de três imagens, que vão direto para o README principal:

1. **`rasterizacao.png`** — a mesma malha desenhada com os três algoritmos, lado a lado. É a imagem mais valiosa do repositório: dá para *enxergar* o serrilhado do Bresenham contra as bordas suavizadas do Xiaolin Wu. Rode `./half-edge-viewer models/modelo.obj 1`, depois `2`, depois `3`, e monte as três num só arquivo.

2. **`transformacoes.gif`** — um GIF curto aplicando rotação, escala e cisalhamento em sequência, terminando com o reset (`0`). Mostra a interatividade em segundos.

3. **`consulta.png`** — um print do terminal ao consultar uma face (tecla `f`), mostrando as adjacências retornadas. É a prova de que a topologia half-edge realmente funciona, não só o desenho.

## Como capturar no Linux

```bash
# print de janela
gnome-screenshot -w

# GIF a partir de uma gravação
peek                                  # grava direto em .gif
# ou
ffmpeg -i captura.mp4 -vf "fps=15,scale=640:-1" docs/transformacoes.gif
```

## Referenciando no README

```markdown
![Comparação dos algoritmos de rasterização](docs/rasterizacao.png)
```
