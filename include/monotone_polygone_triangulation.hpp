/**
 * @file include/monotone_polygone_triangulation.hpp
 * @author Taisiya Osipova
 *
 * @brief Monotone polygone triangulation algorithm implementation.
 */

#ifndef INCLUDE_MONOTONE_POLYGONE_TRIANGULATION_HPP_
#define INCLUDE_MONOTONE_POLYGONE_TRIANGULATION_HPP_

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cmath>
#include <set>
#include <utility>  

struct Point {
    double x, y;
    int id;
    
    explicit Point(double x = 0, double y = 0, int id = -1)
        : x(x), y(y), id(id) {}
    
    bool operator<(const Point& other) const {
        if (y != other.y) return y > other.y;
        return x < other.x;
    }
    
    bool operator==(const Point& other) const {
        return id == other.id;
    }
};

/**
 * @brief Calculate cross product to determine orientation
 *
 * @param a first point
 * @param b second point
 * @param c third point
 *
 * @return cross product
 */
double CrossProduct(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

/**
 * @brief Check if segment is polygon edge
 *
 * @param polygon polygon
 * @param id1 first end of segment
 * @param id2 second end of segment
 *
 * @return true, if segment is polygon edge; false, if not
 */
bool IsPolygonEdge(const std::vector<Point>& polygon, int id1, int id2) {
    for (size_t i = 0; i < polygon.size(); ++i) {
        size_t next = (i + 1) % polygon.size();
        if ((polygon[i].id == id1 && polygon[next].id == id2) ||
            (polygon[i].id == id2 && polygon[next].id == id1)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Triangulate a monotone polygon
 *
 * @param polygon input polygon vertices
 * @return vector of diagonals as vertex index pairs
 */
std::vector<std::pair<int, int>> TriangulateMonotonePolygon(
    const std::vector<Point>& polygon) {
    std::vector<std::pair<int, int>> diagonals;
    if (polygon.size() < 3) return diagonals;

    std::vector<Point> vertices = polygon;
    sort(vertices.begin(), vertices.end());

    int topIdx = 0, bottomIdx = 0;
    for (size_t i = 1; i < polygon.size(); ++i) {
        if (polygon[i].y > polygon[topIdx].y) topIdx = i;
        if (polygon[i].y < polygon[bottomIdx].y) bottomIdx = i;
    }

    std::vector<Point> leftChain, rightChain;

    int current = topIdx;
    do {
        leftChain.push_back(polygon[current]);
        current = (current + 1) % polygon.size();
    } while (current != (bottomIdx + 1) % polygon.size());

    current = topIdx;
    while (current != bottomIdx) {
        rightChain.push_back(polygon[current]);
        current = (current == 0) ? polygon.size() - 1 : current - 1;
    }
    rightChain.push_back(polygon[bottomIdx]);

    std::vector<Point> merged;
    size_t leftPtr = 0, rightPtr = 0;
    while (leftPtr < leftChain.size() && rightPtr < rightChain.size()) {
        if (leftChain[leftPtr].y >= rightChain[rightPtr].y) {
            merged.push_back(leftChain[leftPtr++]);
        } else {
            merged.push_back(rightChain[rightPtr++]);
        }
    }
    while (leftPtr < leftChain.size()) merged.push_back(leftChain[leftPtr++]);
    while (rightPtr < rightChain.size()) merged.push_back(rightChain[rightPtr++]);

    std::stack<Point> S;
    S.push(merged[0]);
    S.push(merged[1]);

    for (size_t i = 2; i < merged.size(); ++i) {
        Point current = merged[i];
        Point top = S.top();

        bool currentInLeft = find(leftChain.begin(), leftChain.end(), current)
            != leftChain.end();
        bool topInLeft = find(leftChain.begin(), leftChain.end(), top)
            != leftChain.end();
        bool sameChain = (currentInLeft && topInLeft) ||
            (!currentInLeft && !topInLeft);

        if (!sameChain) {
            while (!S.empty()) {
                Point next = S.top();
                if (!IsPolygonEdge(polygon, current.id, next.id)) {
                    diagonals.push_back(std::make_pair(current.id, next.id));
                }
                S.pop();
            }
            S.push(current);
        } else {
            while (!S.empty()) {
                Point next = S.top();
                double cross = CrossProduct(top, next, current);
                
                if ((currentInLeft && cross > 0) ||
                    (!currentInLeft && cross < 0)) {
                    if (!IsPolygonEdge(polygon, current.id, next.id)) {
                        diagonals.push_back(std::make_pair(current.id, next.id));
                    }
                    S.pop();
                } else {
                    break;
                }
            }
            S.push(current);
        }
    }

    if (!S.empty()) {
        Point lastVertex = merged.back();
        S.pop();
        
        while (!S.empty()) {
            if (S.size() != 1) {
                diagonals.push_back(std::make_pair(lastVertex.id, S.top().id));
            }
            S.pop();
        }
    }

    return diagonals;
}

#endif  // INCLUDE_MONOTONE_POLYGONE_TRIANGULATION_HPP_
