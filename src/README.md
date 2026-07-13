# src/

Código-fonte. São apenas três arquivos, com responsabilidades separadas.

## `HalfEdge.h` — as structs da topologia

Define os três tipos que formam a estrutura. É deliberadamente mínimo: só os ponteiros, sem lógica.

```cpp
struct Vertex   { float x, y, z;  HalfEdge* edge; };   // uma meia-aresta qualquer que sai daqui
struct Face     { HalfEdge* edge; };                   // uma meia-aresta qualquer do ciclo da face
struct HalfEdge { HalfEdge *twin, *next, *prev;  Vertex* origin;  Face* face; };
```

Repare que `Vertex` e `Face` guardam **uma** meia-aresta apenas. Isso basta: a partir dela, `next` e `twin` alcançam toda a vizinhança. É o ponto principal da estrutura — armazenar pouco e navegar por ponteiros.

## `Mesh.h` — o coração do projeto

A classe `Mesh` concentra tudo. Em ordem de execução:

| Método | O que faz |
| ------ | --------- |
| `loadObj` | Lê `v` (vértices) e `f` (faces) do `.obj`. Lida com o formato `v/vt/vn`, descartando o que vem após a `/`. Índices do `.obj` são 1-based, então subtrai 1. |
| `buildTopology` | **O núcleo.** Cria as meias-arestas, encadeia `next`/`prev` no ciclo de cada face e pareia os `twin` via `edgeMap`. |
| `salvarOriginais` | Guarda as coordenadas iniciais para permitir o reset. |
| `drawLineEquation` / `drawLineBresenham` / `drawLineWu` | Os três rasterizadores. Todos desenham com `GL_POINTS`. |
| `renderMesh` | Percorre as meias-arestas e desenha cada aresta geométrica **uma única vez**, evitando redesenhar a gêmea. |
| `translacao`, `escala`, `rotacao`, `cisalhamento`, `reflexao`, `resetar` | Transformações sobre os vértices. |
| `consultaFace`, `consultaAresta`, `consultaVertice` | As consultas de adjacência — é aqui que a half-edge mostra serventia. |

### Detalhe: o `step` de 0.005

Os rasterizadores trabalham num espaço contínuo (a projeção é `gluOrtho2D(-1, 5, -1, 5)`), não em pixels. O `step = 0.005f` é o tamanho do "pixel virtual": Bresenham e Wu convertem as coordenadas para inteiros dividindo por `step`, rodam o algoritmo em grade inteira e multiplicam de volta ao desenhar.

### Detalhe: percorrer o ciclo de uma face

O padrão que aparece nas consultas — parte de uma meia-aresta e segue `next` até voltar ao começo:

```cpp
HalfEdge* curr = f->edge;
do {
    // ... usa curr ...
    curr = curr->next;
} while (curr != f->edge);
```

## `main.cpp` — interface

Cuida só do GLUT: cria a janela, registra os callbacks de display e teclado, e faz o parsing dos argumentos (`<arquivo.obj> <algoritmo>`). Nenhuma lógica de malha vive aqui.

O fluxo de inicialização é: `loadObj` → `buildTopology` → `salvarOriginais` → `glutMainLoop`.
