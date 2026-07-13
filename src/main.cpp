#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include "Mesh.h"

Mesh malha;

void desenharEixos() {
    glColor3f(0.3f, 0.3f, 0.3f);    
    malha.drawLineEquation(-10.0f, 0.0f, 10.0f, 0.0f); 
    malha.drawLineEquation(0.0f, -10.0f, 0.0f, 10.0f); 
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    desenharEixos();
    malha.renderMesh();
    glutSwapBuffers(); 
}

void init() {
    glClearColor(0, 0, 0, 1);
    glColor3f(1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 5.0, -1.0, 5.0); 
}

void teclado(unsigned char tecla, int x, int y) {
    int id = 0;
    float param1, param2;

    if (tecla == 'f') {
        std::cout << "ID da face: ";
        std::cin >> id;
        if (id >= 0 && id < (int)malha.faces.size()) malha.consultaFace(malha.faces[id]);
        else std::cout << "Face invalida\n";
    } 
    else if (tecla == 'a') {
        std::cout << "ID da aresta: ";
        std::cin >> id;
        if (id >= 0 && id < (int)malha.halfEdges.size()) malha.consultaAresta(malha.halfEdges[id]);
        else std::cout << "Aresta invalida\n";
    } 
    else if (tecla == 'v') {
        std::cout << "ID do vertice: ";
        std::cin >> id;
        if (id >= 0 && id < (int)malha.vertices.size()) malha.consultaVertice(malha.vertices[id]);
        else std::cout << "Vertice invalido\n";
    }
    else if (tecla == 't') {
        std::cout << "Translacao (tx ty): ";
        std::cin >> param1 >> param2;
        malha.translacao(param1, param2);
    }
    else if (tecla == 's') {
        std::cout << "Escala (sx sy): ";
        std::cin >> param1 >> param2;
        malha.escala(param1, param2);
    }
    else if (tecla == 'r') {
        std::cout << "Rotacao em graus: ";
        std::cin >> param1;
        malha.rotacao(param1);
    }
    else if (tecla == 'c') {
        std::cout << "Cisalhamento (shx shy): ";
        std::cin >> param1 >> param2;
        malha.cisalhamento(param1, param2);
    }
    else if (tecla == 'm') {
        std::cout << "Reflexao (1=sim, 0=nao) -> Eixo X, Eixo Y: ";
        int refX, refY;
        std::cin >> refX >> refY;
        malha.reflexao(refX, refY);
    }
    else if (tecla == '0') {
        std::cout << "Objeto resetado para a posicao original.\n";
        malha.resetar();
    }
    else if (tecla == 27 || tecla == 'e') {
        exit(0);
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Faltou passar o .obj ou o algoritmo.\n";
        std::cout << "Uso: ./trabalho5 <arquivo.obj> <algoritmo>\n";
        std::cout << "Algoritmos: 1 (Reta), 2 (Bresenham), 3 (Xiaolin Wu)\n";
        return 1;
    }

    if (!malha.loadObj(argv[1])) {
        std::cout << "Erro ao ler arquivo\n";
        return 1;
    }
    malha.buildTopology();
    malha.salvarOriginais();
    malha.algoritmoRenderizacao = atoi(argv[2]);

    std::cout << "\n=== MENU DE COMANDOS NA JANELA ===\n";
    std::cout << "Consultas:\n 'f' Face | 'a' Aresta | 'v' Vertice\n\n";
    std::cout << "Transformacoes:\n 't' Translacao | 's' Escala | 'r' Rotacao | 'c' Cisalhamento | 'm' Reflexao\n\n";
    std::cout << "Controles:\n '0' Resetar Objeto | 'e' ou ESC Fechar\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);

    glutCreateWindow("Trabalho 5");

    init();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(teclado); 
    
    glutMainLoop();

    return 0;
}