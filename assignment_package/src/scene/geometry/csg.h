#pragma once
#include "shape.h"

enum Operation {PRIMITIVE, UNION , INTER, DIFFER};

class CSGNode
{
public:
    CSGNode(Operation operation);

    Primitive primitive;
    Operation operation;
    CSGNode* leftChild;
    CSGNode* rightChild;
    int index;
};

class CSG : public Primitive
{
public:
    CSG(std::vector<std::shared_ptr<Primitive>> primitives, std::vector<Operation> operations);
    CSGNode* build();
    CSGNode* buildHelper(int* shape_index, int* operation_index);

    virtual bool Intersect(const Ray &ray, Intersection *isect) const;
    bool intersectHelper(CSGNode* node, const Ray &ray, Intersection *isect) const;

    CSGNode* root;
    std::vector<std::shared_ptr<Primitive>> primitives;
    std::vector<Operation> operations;
};

