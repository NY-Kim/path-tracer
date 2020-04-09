#include "csg.h"

CSGNode::CSGNode(Operation operation) : primitive(NULL), leftChild(nullptr), rightChild(nullptr), index(0) {}

CSG::CSG(std::vector<std::shared_ptr<Primitive>> primitives, std::vector<Operation> operations)
    : primitives(primitives), operations(operations)
{
    root = build();
}

CSGNode* CSG::build() {
    int primitive_index = 0;
    int operation_index = 0;
    return buildHelper(&primitive_index, &operation_index);
}

CSGNode* CSG::buildHelper(int *primitive_index, int *operation_index) {
    CSGNode* node = new CSGNode(operations[*operation_index]);
    operation_index++;

    if (operations[*operation_index] == PRIMITIVE) {
        node->index = *primitive_index;
        primitive_index++;
    } else {
        node->leftChild = buildHelper(primitive_index, operation_index);
        node->rightChild = buildHelper(primitive_index, operation_index);
    }

    return node;
}

bool CSG::Intersect(const Ray &ray, Intersection *isect) const {
    return intersectHelper(root, ray, isect);
}

bool CSG::intersectHelper(CSGNode *node, const Ray &ray, Intersection *isect) const {

}
