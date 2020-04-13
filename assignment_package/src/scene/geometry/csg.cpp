#include "csg.h"

CSGNode::CSGNode(Operation operation) : operation(operation), primitive(NULL), leftChild(nullptr), rightChild(nullptr), index(0) {}

void CSGNode::printOperation() {
    if (operation == PRIMITIVE) {
        std::cout << "primitive" << std::endl;
    }
    if (operation == UNION) {
        std::cout << "union" << std::endl;
    }
    if (operation == INTER) {
        std::cout << "inter" << std::endl;
    }
    if (operation == DIFFER) {
        std::cout << "differ" << std::endl;
    }
}

CSG::CSG(std::vector<std::shared_ptr<Primitive>> primitives, std::vector<Operation> operations)
    : primitives(primitives), operations(operations)
{
    root = build();
}

CSGNode* CSG::build() {
    int primitive_index = 0;
    int operation_index = 0;
    CSGNode* node = buildHelper(&primitive_index, &operation_index);
    return node;
}

CSGNode* CSG::buildHelper(int *primitive_index, int *operation_index) {
    if (*operation_index >= operations.size()) {
        return nullptr;
    }
    CSGNode* node = new CSGNode(operations[*operation_index]);

    (*operation_index)++;

    if (node->operation == PRIMITIVE) {
        node->index = *primitive_index;
        (*primitive_index)++;
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
    if (node->operation == PRIMITIVE) {
        bool isects = primitives[node->index]->Intersect(ray, isect);
        return isects;
    }

    Intersection isectLeft, isectRight;
    bool isLeftIntersect = intersectHelper(node->leftChild, ray, &isectLeft);
    bool isRightIntersect = intersectHelper(node->rightChild, ray, &isectRight);

    if (node->operation == UNION) {
        if (isLeftIntersect || isRightIntersect) {
            if (isLeftIntersect && isRightIntersect) {
                *isect = (isectLeft.t < isectRight.t) ? isectLeft : isectRight;
            } else {
                *isect = isLeftIntersect ? isectLeft : isectRight;
            }
            return true;
        }
        return false;
    }

    Intersection isectLeft2 = Intersection();
    Ray rayLeft2 = Ray(ray);
    rayLeft2.origin = ray.origin + ray.direction * (isectLeft.t + 0.001f);
    if (intersectHelper(node->leftChild, rayLeft2, &isectLeft2)) {
        isectLeft2.t += isectLeft.t;
    } else {
        isectLeft2.t = std::numeric_limits<float>::max();
    }

    Intersection isectRight2 = Intersection();
    Ray rayRight2 = Ray(ray);
    rayRight2.origin = ray.origin + ray.direction * (isectRight.t + 0.001f);
    if (intersectHelper(node->rightChild, rayRight2, &isectRight2)) {
        isectRight2.t += isectRight.t;
    } else {
        isectRight2.t = std::numeric_limits<float>::max();
    }

    if (node->operation == INTER) {
        if (isLeftIntersect && isRightIntersect) {
            if (isectLeft.t < isectRight.t && isectLeft2.t < isectRight2.t) { // +A +B -A -B -> N = B, C = A
                *isect = isectRight;
                isect->objectHit = isectLeft.objectHit;
            } else if (isectLeft.t < isectRight.t && isectRight2.t < isectLeft2.t) { // +A +B -B -A -> N = B, C = A
                *isect = isectRight;
                isect->objectHit = isectLeft.objectHit;
            } else if (isectRight.t < isectLeft.t && isectRight2.t < isectLeft2.t) { // +B +A -B -A -> N = A, C = A
                *isect = isectLeft;
            } else if (isectRight.t < isectLeft.t && isectLeft2.t < isectRight2.t) { // +B +A -A -B -> N = A, C = A
                *isect = isectLeft;
            } else {
                return false;
            }
            return true;
        }
        return false;
    }

    if (node->operation == DIFFER) {
        if (isLeftIntersect && isRightIntersect) {
            if (isectRight.t < isectLeft.t && isectRight2.t < isectLeft2.t) { // +B +A -B -A -> N = -B, C = A
                *isect = isectRight;
                isect->objectHit = isectLeft.objectHit;
            } else if (isectRight.t < isectLeft.t && isectLeft2.t < isectRight2.t) { // +B +A -A -B -> null
                return false;
            } else {
                *isect = isectLeft;
            }
            return true;
        } else if (isLeftIntersect) {
            *isect = isectLeft;
            return true;
        }
        return false;
    }
    return false;
}
