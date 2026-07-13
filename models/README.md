# models/

Malhas de entrada no formato Wavefront `.obj`.

## Formato aceito

O parser (`Mesh::loadObj`) lê apenas duas diretivas e ignora o resto do arquivo:

- `v x y z` — um vértice. A coordenada `z` é lida, mas **não é usada**: o projeto é 2D.
- `f i j k ...` — uma face, pelos índices dos seus vértices. Aceita faces com qualquer número de lados (não só triângulos) e aceita o formato `v/vt/vn` (o que vem depois da primeira `/` é descartado).

Índices no `.obj` começam em **1**, não em 0.

## `modelo.obj`

Um quadrado dividido em dois triângulos:

```
v 0.0 0.0 0.0      # v0 ── canto inferior esquerdo
v 2.0 0.0 0.0      # v1 ── canto inferior direito
v 2.0 2.0 0.0      # v2 ── canto superior direito
v 0.0 2.0 0.0      # v3 ── canto superior esquerdo

f 1 2 3            # F0 ── triângulo v0, v1, v2
f 1 3 4            # F1 ── triângulo v0, v2, v3
```

```
  v3──────────v2
   │ ╲    F0  │
   │   ╲      │        a diagonal v0—v2 é compartilhada
   │ F1  ╲    │        pelas duas faces
   │       ╲  │
  v0──────────v1
```

É pequeno de propósito: é o menor exemplo que exercita **os dois casos** que a construção da topologia precisa acertar.

- A diagonal **v0—v2** é uma **aresta interna**: aparece nas duas faces, em direções opostas (`0→2` em F0 e `2→0` em F1). As duas meias-arestas devem se encontrar e virar `twin` uma da outra.
- As quatro arestas da borda aparecem em **uma face só**. Não têm par, então terminam com `twin == nullptr` — e as consultas precisam lidar com isso sem quebrar.

Ou seja: se `modelo.obj` renderiza e as consultas respondem certo, o `buildTopology` está funcionando nos dois caminhos do código.

## Adicionando suas próprias malhas

Qualquer `.obj` exportado do Blender funciona, com duas ressalvas:

1. **Escala.** A câmera é fixa em `gluOrtho2D(-1.0, 5.0, -1.0, 5.0)`. Malhas fora dessa faixa aparecem cortadas ou fora da tela — reposicione com a tecla `t` (translação) e `s` (escala), ou ajuste as coordenadas no arquivo.
2. **Projeção.** Como o `z` é ignorado, um modelo 3D será achatado no plano XY.
