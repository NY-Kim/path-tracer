#include "kdtree.h"

KDNode::KDNode()
    : leftChild(nullptr), rightChild(nullptr), axis(0), minCorner(), maxCorner(), particles()
{}

KDNode::~KDNode()
{
    delete leftChild;
    delete rightChild;
}

KDTree::KDTree()
    : root(nullptr)
{}

KDTree::~KDTree()
{
    delete root;
}

// Comparator functions you can use with std::sort to sort vec3s along the cardinal axes
bool xSort(glm::vec3* a, glm::vec3* b) { return a->x < b->x; }
bool ySort(glm::vec3* a, glm::vec3* b) { return a->y < b->y; }
bool zSort(glm::vec3* a, glm::vec3* b) { return a->z < b->z; }


void KDTree::build(const std::vector<glm::vec3*> &points)
{
    //TODO
}

std::vector<glm::vec3> KDTree::particlesInSphere(glm::vec3 c, float r)
{
    //TODO
    return std::vector<glm::vec3>();
}

void KDTree::clear()
{
    delete root;
    root = nullptr;
}
