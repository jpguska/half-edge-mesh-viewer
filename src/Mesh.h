#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <GL/gl.h>
#include "HalfEdge.h"

class Mesh {
public:
    std::vector<Vertex*> vertices;
    std::vector<Face*> faces;
    std::vector<HalfEdge*> halfEdges;
    std::vector<std::vector<int>> faceIndices;
    std::vector<std::pair<float, float>> originais;

    int algoritmoRenderizacao = 1; // 1=Reta, 2=Bresenham, 3=Wu

    Mesh() {}
    
    ~Mesh() {
        for (auto v : vertices) delete v;
        for (auto f : faces) delete f;
        for (auto he : halfEdges) delete he;
    }

    bool loadObj(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                vertices.push_back(new Vertex(x, y, z));
            } 
            else if (prefix == "f") {
                std::vector<int> currentFace;
                std::string vertexStr;
                
                while (iss >> vertexStr) {
                    size_t pos = vertexStr.find('/');
                    int vIdx = std::stoi(vertexStr.substr(0, pos)) - 1; 
                    currentFace.push_back(vIdx);
                }
                faceIndices.push_back(currentFace);
            }
        }
        file.close();
        return true;
    }

    void buildTopology() {
        std::map<std::pair<int, int>, HalfEdge*> edgeMap;

        for (const auto& indices : faceIndices) {
            int n = indices.size();
            if (n < 3) continue;

            Face* newFace = new Face();
            faces.push_back(newFace);
            std::vector<HalfEdge*> faceEdges;

            for (int i = 0; i < n; ++i) {
                HalfEdge* he = new HalfEdge();
                halfEdges.push_back(he);
                faceEdges.push_back(he);

                int currIdx = indices[i];
                int nextIdx = indices[(i + 1) % n];

                he->origin = vertices[currIdx];
                he->face = newFace;

                if (!he->origin->edge) he->origin->edge = he;

                std::pair<int, int> twinKey = {nextIdx, currIdx};
                if (edgeMap.count(twinKey)) {
                    HalfEdge* twinEdge = edgeMap[twinKey];
                    he->twin = twinEdge;
                    twinEdge->twin = he;
                } else {
                    edgeMap[{currIdx, nextIdx}] = he;
                }
            }

            for (int i = 0; i < n; ++i) {
                faceEdges[i]->next = faceEdges[(i + 1) % n];
                faceEdges[i]->prev = faceEdges[(i - 1 + n) % n];
            }

            newFace->edge = faceEdges[0];
        }
    }

    void salvarOriginais() {
        originais.clear();
        for (Vertex* v : vertices) {
            originais.push_back({v->x, v->y});
        }
    }

    void resetar() {
        for (size_t i = 0; i < vertices.size(); i++) {
            vertices[i]->x = originais[i].first;
            vertices[i]->y = originais[i].second;
        }
    }

    void drawLineEquation(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        float step = 0.005f; 

        glBegin(GL_POINTS);

        if (std::abs(dx) >= std::abs(dy)) {
            if (x1 > x2) { std::swap(x1, x2); std::swap(y1, y2); }
            float m = dy / dx;
            float b = y1 - m * x1;
        
            for (float x = x1; x <= x2; x += step) {
                glVertex2f(x, m * x + b);
            }
        } else {
            if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); }
        
            if (dx == 0.0f) {
                for (float y = y1; y <= y2; y += step) glVertex2f(x1, y);
            } else {
                float m = dy / dx;
                float b = y1 - m * x1;
                for (float y = y1; y <= y2; y += step) {
                    glVertex2f((y - b) / m, y);
                }
            }
        }
        glEnd();
    }

    void drawLineBresenham(float x1, float y1, float x2, float y2) {
        float step = 0.005f;
        int x0 = std::round(x1 / step);
        int y0 = std::round(y1 / step);
        int x_end = std::round(x2 / step);
        int y_end = std::round(y2 / step);

        int dx = std::abs(x_end - x0), sx = x0 < x_end ? 1 : -1;
        int dy = std::abs(y_end - y0), sy = y0 < y_end ? 1 : -1;
        int err = (dx > dy ? dx : -dy) / 2, e2;

        glBegin(GL_POINTS);
        while (true) {
            glVertex2f(x0 * step, y0 * step);
            if (x0 == x_end && y0 == y_end) break;
            e2 = err;
            if (e2 > -dx) { err -= dy; x0 += sx; }
            if (e2 < dy) { err += dx; y0 += sy; }
        }
        glEnd();
    }

    void drawLineWu(float x1, float y1, float x2, float y2) {
        float step = 0.005f;
        float x0 = x1 / step, y0 = y1 / step;
        float x_end = x2 / step, y_end = y2 / step;

        auto ipart = [](float x) -> int { return std::floor(x); };
        auto fpart = [](float x) -> float { return x - std::floor(x); };
        auto rfpart = [&](float x) -> float { return 1.0f - fpart(x); };

        bool steep = std::abs(y_end - y0) > std::abs(x_end - x0);
        if (steep) {
            std::swap(x0, y0);
            std::swap(x_end, y_end);
        }
        if (x0 > x_end) {
            std::swap(x0, x_end);
            std::swap(y0, y_end);
        }

        float dx = x_end - x0;
        float dy = y_end - y0;
        float gradient = (dx == 0.0f) ? 1.0f : dy / dx;
        float intersectY = y0;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_POINTS);

        for (int x = ipart(x0); x <= ipart(x_end); x++) {
            if (steep) {
                glColor4f(1.0f, 1.0f, 1.0f, rfpart(intersectY));
                glVertex2f(ipart(intersectY) * step, x * step);
                glColor4f(1.0f, 1.0f, 1.0f, fpart(intersectY));
                glVertex2f((ipart(intersectY) + 1) * step, x * step);
            } else {
                glColor4f(1.0f, 1.0f, 1.0f, rfpart(intersectY));
                glVertex2f(x * step, ipart(intersectY) * step);
                glColor4f(1.0f, 1.0f, 1.0f, fpart(intersectY));
                glVertex2f(x * step, (ipart(intersectY) + 1) * step);
            }
            intersectY += gradient;
        }

        glEnd();
        glDisable(GL_BLEND);
    }

    void renderMesh() {
        glColor3f(1.0f, 1.0f, 1.0f);
        for (HalfEdge* he : halfEdges) {
            if (!he->twin || he < he->twin) {
                Vertex* v1 = he->origin;
                Vertex* v2 = he->next->origin; 
                
                if (algoritmoRenderizacao == 1) drawLineEquation(v1->x, v1->y, v2->x, v2->y);
                else if (algoritmoRenderizacao == 2) drawLineBresenham(v1->x, v1->y, v2->x, v2->y);
                else if (algoritmoRenderizacao == 3) drawLineWu(v1->x, v1->y, v2->x, v2->y);
            }
        }
    }

    void translacao(float tx, float ty) {
        for (Vertex* v : vertices) {
            v->x += tx;
            v->y += ty;
        }
    }

    void escala(float sx, float sy) {
        for (Vertex* v : vertices) {
            v->x *= sx;
            v->y *= sy;
        }
    }

    void rotacao(float angulo) {
        float rad = angulo * M_PI / 180.0f;
        float cosA = std::cos(rad);
        float sinA = std::sin(rad);

        for (Vertex* v : vertices) {
            float nx = v->x * cosA - v->y * sinA;
            float ny = v->x * sinA + v->y * cosA;
            v->x = nx;
            v->y = ny;
        }
    }

    void cisalhamento(float shx, float shy) {
        for (Vertex* v : vertices) {
            float nx = v->x + shx * v->y;
            float ny = v->y + shy * v->x;
            v->x = nx;
            v->y = ny;
        }
    }

    void reflexao(bool eixoX, bool eixoY) {
        for (Vertex* v : vertices) {
            if (eixoX) v->x = -v->x;
            if (eixoY) v->y = -v->y;
        }
    }

    int getFaceID(Face* f) { return f ? std::distance(faces.begin(), std::find(faces.begin(), faces.end(), f)) : -1; }
    int getVertexID(Vertex* v) { return v ? std::distance(vertices.begin(), std::find(vertices.begin(), vertices.end(), v)) : -1; }
    int getEdgeID(HalfEdge* he) { return he ? std::distance(halfEdges.begin(), std::find(halfEdges.begin(), halfEdges.end(), he)) : -1; }

    void consultaFace(Face* f) {
        if (!f) return;
        std::cout << "\n[Face " << getFaceID(f) << "]\nVertices: ";
        HalfEdge* curr = f->edge;
        do {
            std::cout << getVertexID(curr->origin) << " ";
            curr = curr->next;
        } while (curr != f->edge);
        
        std::cout << "\nArestas: ";
        curr = f->edge;
        do {
            std::cout << getEdgeID(curr) << " ";
            curr = curr->next;
        } while (curr != f->edge);

        std::cout << "\nFaces adjacentes: ";
        curr = f->edge;
        do {
            if (curr->twin && curr->twin->face) std::cout << getFaceID(curr->twin->face) << " ";
            curr = curr->next;
        } while (curr != f->edge);
        std::cout << "\n";
    }

    void consultaAresta(HalfEdge* he) {
        if (!he) return;
        std::cout << "\n[Aresta " << getEdgeID(he) << "]\nFaces adjacentes: " << getFaceID(he->face);
        if (he->twin) std::cout << ", " << getFaceID(he->twin->face);
        std::cout << "\nVertices: inicio " << getVertexID(he->origin) << ", fim " << getVertexID(he->next->origin);
        std::cout << "\nArestas adjacentes: " << getEdgeID(he->prev) << " (prev), " << getEdgeID(he->next) << " (next)\n";
    }

    void consultaVertice(Vertex* v) {
        if (!v) return;
        std::cout << "\n[Vertice " << getVertexID(v) << "]\nArestas incidentes: ";
        
        for (HalfEdge* he : halfEdges) {
            if (he->origin == v) {
                std::cout << getEdgeID(he) << " ";
            }
        }

        std::cout << "\nFaces adjacentes: ";
        for (HalfEdge* he : halfEdges) {
            if (he->origin == v && he->face) {
                std::cout << getFaceID(he->face) << " ";
            }
        }
        std::cout << "\n";
    }
};

#endif