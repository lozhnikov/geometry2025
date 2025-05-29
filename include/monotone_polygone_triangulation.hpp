/**
 * @file include/monotone_polygone_triangulation.hpp
 * @author Taisiya Osipova
 *
 * @brief Monotone polygone triangulation algorithm implementation.
 */

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;

struct Point {
    double x, y;
    int id;
    
    Point(double x = 0, double y = 0, int id = -1) : x(x), y(y), id(id) {}
    
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
 * @return true, if segment is polygon edge; fase, if not
 */
bool IsPolygonEdge(const vector<Point>& polygon, int id1, int id2) {
    for (size_t i = 0; i < polygon.size(); ++i) {
        size_t next = (i + 1) % polygon.size();
        if ((polygon[i].id == id1 && polygon[next].id == id2) ||
            (polygon[i].id == id2 && polygon[next].id == id1)) {
            return true;
        }
    }
    return false;
}


vector<pair<int, int> > TriangulateMonotonePolygon(const vector<Point>& polygon) {
    vector<pair<int, int> > diagonals;
    if (polygon.size() < 3) return diagonals;

    vector<Point> vertices = polygon;
    sort(vertices.begin(), vertices.end());

    int topIdx = 0, bottomIdx = 0;
    for (size_t i = 1; i < polygon.size(); ++i) {
        if (polygon[i].y > polygon[topIdx].y) topIdx = i;
        if (polygon[i].y < polygon[bottomIdx].y) bottomIdx = i;
    }

    vector<Point> leftChain, rightChain;

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

    vector<Point> merged;
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

    stack<Point> S;
    S.push(merged[0]);
    S.push(merged[1]);

    for (size_t i = 2; i < merged.size(); ++i) {
        Point current = merged[i];
        Point top = S.top();

        bool currentInLeft = find(leftChain.begin(), leftChain.end(), current) != leftChain.end();
        bool topInLeft = find(leftChain.begin(), leftChain.end(), top) != leftChain.end();
        bool sameChain = (currentInLeft && topInLeft) || (!currentInLeft && !topInLeft);

        if (!sameChain) {
            // cout << "Разные цепи - добавляем диагонали ко всем вершинам в стеке\n";
            while (!S.empty()) {
                Point next = S.top();
                // cout << "  Проверяем вершину: " << next.id << "(" << next.x << "," << next.y << ")\n";
                if (!IsPolygonEdge(polygon, current.id, next.id)) {
                    // cout << "  Добавляем диагональ: " << current.id << " - " << next.id << "\n";
                    diagonals.push_back(make_pair(current.id, next.id));
                }
                S.pop();
            }
            S.push(current);
        } else {
            // cout << "Одинаковые цепи - проверяем возможность добавления диагоналей\n";
            while (!S.empty()) {
                Point next = S.top();
                double cross = CrossProduct(top, next, current);
                
                // cout << "  Проверяем вершину: " << next.id << "(" << next.x << "," << next.y << ")\n";
                // cout << "  Векторное произведение (top=" << top.id << ", next=" << next.id
                //      << ", current=" << current.id << "): " << cross << "\n";
                
                if ((currentInLeft && cross > 0) || (!currentInLeft && cross < 0)) {
                    if (!IsPolygonEdge(polygon, current.id, next.id)) {
                        // cout << "  Условие выполнено, добавляем диагональ: "
                        //      << current.id << " - " << next.id << "\n";
                        diagonals.push_back(make_pair(current.id, next.id));
                    } else {
                        // cout << "  Условие выполнено, но это ребро многоугольника - пропускаем\n";
                    }
                    S.pop();
                } else {
                    // cout << "  Условие не выполнено - прерываем цикл\n";
                    break;
                }
            }
            S.push(current);
        }
    }

    // завершающая фаза обработки стека
    if (!S.empty()) {
        Point lastVertex = merged.back();
        S.pop(); // Удаляем верхний элемент
        
        while (!S.empty()) {
            if (S.size() != 1) {
                diagonals.push_back(make_pair(lastVertex.id, S.top().id));
            }
            S.pop();
        }
    }

    return diagonals;
}

/*int main() {
    vector<Point> polygon;
    polygon.push_back(Point(0, 4, 0));
    polygon.push_back(Point(2, 3, 1));
    polygon.push_back(Point(1, 1, 2));
    polygon.push_back(Point(4, 0, 3));
    polygon.push_back(Point(0, 0, 4));
    
    cout << "Триангуляция монотонного многоугольника 1:\n";
    vector<pair<int, int> > diagonals = TriangulateMonotonePolygon(polygon);

    for (size_t i = 0; i < diagonals.size(); ++i) {
        if (!IsPolygonEdge(polygon, diagonals[i].first, diagonals[i].second) &&
            diagonals[i].first != diagonals[i].second) {
            cout << diagonals[i].first << " - " << diagonals[i].second << endl;
        }
    }
    
    vector<Point> polygon2;
    polygon2.push_back(Point(0, 0, 0));
    polygon2.push_back(Point(-2, 2, 1));
    polygon2.push_back(Point(0, 1, 2));
    polygon2.push_back(Point(1, 2, 3));
    
    cout << "Триангуляция монотонного многоугольника 2:\n";
    vector<pair<int, int> > diagonals2 = TriangulateMonotonePolygon(polygon2);

    for (size_t i = 0; i < diagonals2.size(); ++i) {
        if (!IsPolygonEdge(polygon2, diagonals2[i].first, diagonals2[i].second) &&
            diagonals2[i].first != diagonals2[i].second) {
            cout << diagonals2[i].first << " - " << diagonals2[i].second << endl;
        }
    }
    
    vector<Point> polygon3;
    polygon3.push_back(Point(0, 0, 0));
    polygon3.push_back(Point(1, 3, 1));
    polygon3.push_back(Point(4, 2, 2));
    polygon3.push_back(Point(5, 1, 3));
    
    cout << "Триангуляция монотонного многоугольника 3:\n";
    vector<pair<int, int> > diagonals3 = TriangulateMonotonePolygon(polygon3);

    for (size_t i = 0; i < diagonals3.size(); ++i) {
        if (!IsPolygonEdge(polygon3, diagonals3[i].first, diagonals3[i].second) &&
            diagonals3[i].first != diagonals3[i].second) {
            cout << diagonals3[i].first << " - " << diagonals3[i].second << endl;
        }
    }
    
    vector<Point> polygon4;
    polygon4.push_back(Point(0, 2, 0));
    polygon4.push_back(Point(2, 0, 1));
    polygon4.push_back(Point(0, -2, 2));
    polygon4.push_back(Point(-2, 0, 3));
    
    cout << "Триангуляция монотонного многоугольника 4:\n";
    vector<pair<int, int> > diagonals4 = TriangulateMonotonePolygon(polygon4);

    for (size_t i = 0; i < diagonals4.size(); ++i) {
        if (!IsPolygonEdge(polygon4, diagonals4[i].first, diagonals4[i].second) &&
            diagonals4[i].first != diagonals4[i].second) {
            cout << diagonals4[i].first << " - " << diagonals4[i].second << endl;
        }
    }
    
    vector<Point> polygon5;
    polygon5.push_back(Point(0, 0, 0));
    polygon5.push_back(Point(1, 5, 1));
    polygon5.push_back(Point(3, 4, 2));
    polygon5.push_back(Point(5, 2, 3));
    polygon5.push_back(Point(6, 0, 4));
    
    cout << "Триангуляция монотонного многоугольника 5:\n";
    vector<pair<int, int> > diagonals5 = TriangulateMonotonePolygon(polygon5);

    for (size_t i = 0; i < diagonals5.size(); ++i) {
        if (!IsPolygonEdge(polygon5, diagonals5[i].first, diagonals5[i].second) &&
            diagonals5[i].first != diagonals5[i].second) {
            cout << diagonals5[i].first << " - " << diagonals5[i].second << endl;
        }
    }

    return 0;
}*/
