/** 
 * @file include/shell_merge.hpp 
 * @author Щербинин Виталий
 * 
 * @brief Реализация алгоритма слияния двух выпуклых оболочек.
 */

#ifndef INCLUDE_SHELL_MERGE_HPP_
#define INCLUDE_SHELL_MERGE_HPP_

#include <vector>
#include <iostream>
#include <algorithm>

namespace geometry {

/**
 * @brief Структура точки на плоскости.
 */
struct Point {
  double x, y;
  explicit Point(double xx = 0, double yy = 0) : x(xx), y(yy) {}
};

/**
 * @brief Узел (вершина) в связном списке точек.
 */
struct Vertex {
  Point pt;
  Vertex* next;

  explicit Vertex(const Point& p) : pt(p), next(nullptr) {}
};

/**
 * @brief Представление выпуклого многоугольника с помощью связного списка вершин.
 */
class Polygon {
 public:
  Vertex* head;

  Polygon() : head(nullptr) {}

/**
 * @brief Вставляет точку в конец списка вершин.
 * @param p точка
 */

  void insert(const Point& p) {
    if (!head) {
      head = new Vertex(p);
      head->next = head;
      return;
    }
    Vertex* v = new Vertex(p);
    Vertex* cur = head;
    while (cur->next != head)
      cur = cur->next;
    cur->next = v;
    v->next = head;
  }
/**
 * @brief Печать всех вершин мноугольника.
 */
  void print() const {
    if (!head) return;
    Vertex* v = head;
    do {
      std::cout << "(" << v->pt.x << ", " << v->pt.y << ") ";
      v = v->next;
    } while (v != head);
    std::cout << std::endl;
  }
};

/**
 * @param a Первая точка
 * @param b Вторая точка
 * @param c Третья точка
 * @brief Возвращает ориентацию трёх точек (по знаку векторного произведения).
 * @return 1 (левый поворот), -1 (правый поворот), 0 (коллинеарны)
 */
inline int orientation(const Point& a, const Point& b, const Point& c) {
  double val = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);
  if (val > 0) return 1;
  if (val < 0) return -1;
  return 0;
}

/**
 * @brief Строит выпуклую оболочку из двух выпуклых многоугольников.
 * @param L Указатель на левый выпуклый многоугольник.
 * @param R Указатель на правый выпуклый многоугольник.
 * @return Новый многоугольник — выпуклая оболочка объединённого множества.
 */
inline Polygon* MergeHulls(const Polygon* L, const Polygon* R) {
  std::vector<Point> combined;

  if (L && L->head) {
  Vertex* v = L->head;
  do {
    combined.push_back(v->pt);
    v = v->next;
  } while (v != L->head);
  }

  if (R && R->head) {
  Vertex* v = R->head;
  do {
    combined.push_back(v->pt);
    v = v->next;
  } while (v != R->head);
  }
  std::sort(combined.begin(), combined.end(),
   [](const Point& a, const Point& b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
  });

  std::vector<Point> hull;

  // Нижняя часть
  for (const auto& p : combined) {
    while (hull.size() >= 2 &&
           orientation(hull[hull.size() - 2], hull.back(), p) <= 0)
      hull.pop_back();
    hull.push_back(p);
  }

  // Верхняя часть
  size_t t = hull.size() + 1;
  for (int i = static_cast<int>(combined.size()) - 2; i >= 0; i--) {
    while (hull.size() >= t &&
           orientation(hull[hull.size() - 2], hull.back(), combined[i]) <= 0)
      hull.pop_back();
    hull.push_back(combined[i]);
  }

  hull.pop_back();  // последняя дублирует первую

  Polygon* result = new Polygon();
  for (const auto& p : hull)
    result->insert(p);

  return result;
}

}  // namespace geometry

#endif  // INCLUDE_SHELL_MERGE_HPP_
