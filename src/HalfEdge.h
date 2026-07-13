#ifndef HALF_EDGE_H
#define HALF_EDGE_H

struct HalfEdge;
struct Face;

struct Vertex {
    float x, y, z;  
    HalfEdge* edge; 

    Vertex(float x, float y, float z = 0.0f) : x(x), y(y), z(z), edge(nullptr) {}
};

struct Face {
    HalfEdge* edge; 

    Face() : edge(nullptr) {}
};

struct HalfEdge {
    HalfEdge* twin;   
    HalfEdge* next;   
    HalfEdge* prev;   
    Vertex* origin;   
    Face* face;       

    HalfEdge() : twin(nullptr), next(nullptr), prev(nullptr), origin(nullptr), face(nullptr) {}
};

#endif