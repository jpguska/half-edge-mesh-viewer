# Half-Edge Mesh Viewer

Visualizador de malhas 2D que constrói a topologia de um arquivo `.obj` usando a estrutura de dados **half-edge**, rasteriza as arestas com três algoritmos implementados do zero e aplica transformações geométricas interativas.

Trabalho da disciplina de **Computação Gráfica** — UTFPR Toledo, 2026.1.
Autores: **Lucas Lima** e **João Pedro Guska**.

> Todo o desenho é feito com `GL_POINTS`. Nenhuma primitiva de linha do OpenGL (`GL_LINES`) é utilizada — cada pixel da aresta é calculado pelos algoritmos de rasterização implementados aqui.

---

## Por que half-edge?

Um arquivo `.obj` guarda apenas geometria: uma lista de vértices e, para cada face, os índices dos vértices que a compõem. Ele não diz **quem é vizinho de quem**. Perguntas como *"quais faces compartilham esta aresta?"* ou *"quais arestas incidem neste vértice?"* exigiriam varrer a lista inteira de faces a cada consulta.

A estrutura **half-edge** resolve isso quebrando cada aresta em duas metades opostas. Cada metade guarda quatro ponteiros:

| Campo    | Aponta para                                         |
| -------- | --------------------------------------------------- |
| `origin` | vértice de onde a meia-aresta sai                   |
| `face`   | face à qual ela pertence                            |
| `next`   | próxima meia-aresta no ciclo daquela face           |
| `prev`   | meia-aresta anterior no ciclo                       |
| `twin`   | a meia-aresta oposta (da face vizinha)              |

Com isso, percorrer a vizinhança vira um passeio por ponteiros: `next` circula dentro de uma face, `twin` atravessa para a face vizinha.

```
        v2
        /\
       /  \
   he1/    \he0        he0 e he0->twin são as duas metades
     /  F0  \          da mesma aresta geométrica (v0—v2).
    /        \         he0->twin pertence à face vizinha F1.
  v0──────────v1
       he2
```

### Como os `twin` são pareados

O ponto central da construção (`Mesh::buildTopology`) é o pareamento das metades. Ao criar a meia-aresta que vai de `a` para `b`, procuramos num mapa se já existe uma registrada como `(b, a)` — a direção oposta. Se existe, as duas são gêmeas; senão, registramos `(a, b)` e esperamos que a gêmea apareça quando a face vizinha for processada.

```cpp
std::pair<int, int> twinKey = {nextIdx, currIdx};   // procura a direção oposta
if (edgeMap.count(twinKey)) {
    he->twin = edgeMap[twinKey];
    edgeMap[twinKey]->twin = he;                    // pareamento mútuo
} else {
    edgeMap[{currIdx, nextIdx}] = he;               // aguarda a gêmea
}
```

Arestas de borda nunca encontram par, então terminam com `twin == nullptr` — o que é o comportamento correto e é tratado nas consultas.

---

## Algoritmos de rasterização

Os três são selecionados por argumento de linha de comando e desenham a **mesma** malha, permitindo comparação visual direta.

**1 — Equação da reta.** Percorre o eixo dominante e calcula o outro por `y = mx + b`. Simples, mas usa aritmética de ponto flutuante a cada passo. O eixo de iteração é escolhido pelo maior delta, evitando falhas em retas quase verticais.

**2 — Bresenham.** Substitui o ponto flutuante por **aritmética inteira** e um acumulador de erro: a cada passo decide se avança apenas no eixo dominante ou nos dois. Mais rápido e sem erro de arredondamento acumulado.

**3 — Xiaolin Wu.** Faz **antialiasing**: em vez de escolher *um* pixel, acende os dois pixels vizinhos com intensidades proporcionais à distância da reta ideal (`fpart` / `rfpart`), via alpha blending. Bordas visivelmente mais suaves, ao custo de dobrar os pontos desenhados.

---

## Transformações geométricas

Aplicadas diretamente sobre as coordenadas dos vértices, em 2D:

| Tecla | Transformação | Parâmetros |
| ----- | ------------- | ---------- |
| `t`   | Translação    | `tx ty`    |
| `s`   | Escala        | `sx sy`    |
| `r`   | Rotação       | ângulo em graus |
| `c`   | Cisalhamento  | `shx shy`  |
| `m`   | Reflexão      | `1/0` para eixo X, `1/0` para eixo Y |
| `0`   | Resetar para a posição original | — |

As posições originais são salvas na carga (`salvarOriginais`), então `0` sempre devolve a malha ao estado inicial, por mais transformações que tenham sido encadeadas.

## Consultas topológicas

| Tecla | Consulta | Retorna |
| ----- | -------- | ------- |
| `f`   | Face     | vértices, arestas e faces adjacentes |
| `a`   | Aresta   | faces adjacentes, vértices inicial/final, arestas `prev`/`next` |
| `v`   | Vértice  | arestas incidentes e faces adjacentes |

---

## Como compilar e executar

**Dependências:** OpenGL, GLU e GLUT.

```bash
# Debian/Ubuntu
sudo apt install freeglut3-dev

# compilar
make

# executar: <arquivo.obj> <algoritmo>
./half-edge-viewer models/modelo.obj 2
```

O segundo argumento seleciona o algoritmo: `1` = equação da reta, `2` = Bresenham, `3` = Xiaolin Wu.

Sem o Makefile:

```bash
g++ src/main.cpp -o half-edge-viewer -lGL -lGLU -lglut
```

**Uso:** as teclas são pressionadas **na janela gráfica**; os valores são digitados **no terminal**. `e` ou `ESC` encerra.

---

## Estrutura do projeto

```
half-edge-mesh-viewer/
├── src/
│   ├── HalfEdge.h    # structs Vertex, Face e HalfEdge (os ponteiros da topologia)
│   ├── Mesh.h        # carga do .obj, buildTopology, rasterização, transformações, consultas
│   └── main.cpp      # janela GLUT, loop de render e tratamento de teclado
├── models/
│   └── modelo.obj    # malha de exemplo
├── docs/             # capturas de tela
└── Makefile
```

---

## Limitações conhecidas

Registradas de forma honesta — este é um trabalho de disciplina, não uma engine.

- **É 2D.** A coordenada `z` do `.obj` é lida mas não usada. Não há câmera, projeção perspectiva, iluminação nem preenchimento de faces — apenas wireframe sobre uma projeção ortográfica fixa (`gluOrtho2D`).
- **Sem preenchimento de faces.** A estrutura half-edge suporta, mas não foi implementado.
- **Malhas *manifold* apenas.** Se três ou mais faces compartilharem a mesma aresta, o pareamento de `twin` fica indefinido.
- **`renderMesh` usa comparação de ponteiros** (`he < he->twin`) para desenhar cada aresta uma única vez. Funciona na prática, mas comparar ponteiros de alocações independentes é *unspecified behavior* em C++ — o correto seria comparar os índices das meias-arestas.
- **Consultas são O(n).** `getFaceID`/`getVertexID`/`getEdgeID` fazem busca linear com `std::find`. Guardar o índice dentro de cada struct resolveria.

## Possíveis melhorias

- Estender para 3D com câmera e projeção perspectiva
- Preenchimento de faces (scanline) usando a topologia já disponível
- Operações de edição de malha que a half-edge viabiliza: *edge flip*, *edge collapse*, subdivisão
- Substituir os ponteiros crus por índices e `std::vector`, eliminando o gerenciamento manual de memória

---

## Autoria

Projeto desenvolvido para a disciplina de **Computação Gráfica** por João Guska e Lucas Colombani.
