#include "kdtree.h"
#include <iostream>

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
    std::vector<glm::vec3> minmax = getMinMax(points);
    minCorner = minmax[0];
    maxCorner = minmax[1];
    root = buildHelper(points, 0, minCorner, maxCorner);
}

std::vector<glm::vec3> KDTree::getMinMax(const std::vector<glm::vec3*> &points) {
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();

    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();
    float maxZ = std::numeric_limits<float>::min();

    for (int i = 0; i < points.size(); i++) {
        glm::vec3 point = *points[i];
        if (point.x < minX) {
            minX = point.x;
        }
        if (point.y < minY) {
            minY = point.y;
        }
        if (point.z < minZ) {
            minZ = point.z;
        }
        if (point.x > maxX) {
            maxX = point.x;
        }
        if (point.y > maxY) {
            maxY = point.y;
        }
        if (point.z > maxZ) {
            maxZ = point.z;
        }
    }

    std::vector<glm::vec3> minmax;
    minmax.push_back(glm::vec3(minX, minY, minZ));
    minmax.push_back(glm::vec3(maxX, maxY, maxZ));

    return minmax;
}

KDNode* KDTree::buildHelper(const std::vector<glm::vec3*> &points, int depth, glm::vec3 minCorner, glm::vec3 maxCorner)
{
    KDNode* node = new KDNode();
    node->minCorner = minCorner;
    node->maxCorner = maxCorner;
    node->axis = depth % 3;

    if (points.size() == 1) {
        node->particles = points;
        return node;
    }

    unsigned int size = points.size();
    unsigned int axis = depth % 3;
    std::vector<glm::vec3*> sortedPoints;
    for (int i = 0; i < size; i++) {
        sortedPoints.push_back(points[i]);
    }

    if (axis == 0) {
        std::sort(sortedPoints.begin(), sortedPoints.end(), xSort);
    } else if (axis == 1) {
        std::sort(sortedPoints.begin(), sortedPoints.end(), ySort);
    } else {
        std::sort(sortedPoints.begin(), sortedPoints.end(), zSort);
    }

    std::vector<glm::vec3*> leftPoints(sortedPoints.begin(), sortedPoints.begin() + size / 2);
    std::vector<glm::vec3*> rightPoints(sortedPoints.begin() + size / 2, sortedPoints.end());

    std::vector<glm::vec3> leftMinmax = getMinMax(leftPoints);
    std::vector<glm::vec3> rightMinmax = getMinMax(rightPoints);
    node->leftChild  = buildHelper(leftPoints, depth + 1, leftMinmax[0], leftMinmax[1]);
    node->rightChild = buildHelper(rightPoints, depth + 1, rightMinmax[0], rightMinmax[1]);

    return node;
}



std::vector<glm::vec3> KDTree::particlesInSphere(glm::vec3 c, float r)
{
    std::vector<glm::vec3*> points;
    search(&points, root, c, r);
    std::vector<glm::vec3> returnPoints;
    for (int i = 0; i < points.size(); i++) {
        returnPoints.push_back(*points[i]);
    }
    return returnPoints;
}

void KDTree::search(std::vector<glm::vec3*> *points, KDNode* node, glm::vec3 c, float r)
{
    if (!node) {
        return;
    } else if (!node->leftChild && !node->rightChild) {
        glm::vec3 *point = node->particles[0];
        if (glm::distance(*point, c) <= r) {
            points->push_back(point);
        }
        return;
    } else {
        glm::vec3 min = c - glm::vec3(r, r, r);
        glm::vec3 max = c + glm::vec3(r, r, r);
        if ((node->maxCorner.x >= min.x && node->minCorner.x <= max.x) &&
            (node->maxCorner.y >= min.y && node->minCorner.y <= max.y) &&
            (node->maxCorner.z >= min.z && node->minCorner.z <= max.z)) {
            search(points, node->leftChild, c, r);
            search(points, node->rightChild, c, r);
        }
    }
}

void KDTree::clear()
{
    delete root;
    root = nullptr;
}
