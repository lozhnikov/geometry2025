/**
 * @brief Template implementation of Closest Pair of Points algorithm
 */

#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>

/**
 * @brief Templated Point structure
 */
template<typename T>
struct Point {
  T x, y;
  explicit Point(T x = T(), T y = T()) : x(x), y(y) {}
};

/**
 * @brief Edge representing a pair of points
 */

template<typename T>
struct Edge {
  Point<T> p1, p2;
  explicit Edge(Point<T> p1 = Point<T>{}, Point<T> p2 = Point<T>{}) : p1(p1), p2(p2) {}

  T length() const {
    T dx = p1.x - p2.x;
    T dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy);
  }
};

/**
 * @brief Helper comparator for sorting points by X
 */
template<typename T>
bool cmpX(const Point<T>* a, const Point<T>* b) {
  return a->x < b->x;
}

/**
 * @brief Helper comparator for sorting points by Y
 */
template<typename T>
bool cmpY(const Point<T>* a, const Point<T>* b) {
  return (a->y < b->y) || (std::abs(a->y - b->y) < 1e-10 && a->x < b->x);
}

/**
 * @brief Split Y-array for merge step
 */
template<typename T>
void splitY(const std::vector<Point<T>*>& y,
T xMid, std::vector<Point<T>*>& yL, std::vector<Point<T>*>& yR) {
  for (auto p : y) {
    (p->x < xMid ? yL : yR).push_back(p);
  }
}

/**
 * @brief Check strip points for closer pair
 */
template<typename T>
T checkStrip(const std::vector<Point<T>*>& strip, T delta, Edge<T>& closest) {
  T minDist = delta;
  for (size_t i = 0; i < strip.size(); ++i) {
    for (size_t j = i + 1; j < strip.size() &&
      (strip[j]->y - strip[i]->y) < minDist; ++j) {
      T dx = strip[i]->x - strip[j]->x;
      T dy = strip[i]->y - strip[j]->y;
      T dist = std::sqrt(dx * dx + dy * dy);
      if (dist < minDist) {
        minDist = dist;
        closest = Edge<T>(*strip[i], *strip[j]);
      }
    }
  }
  return minDist;
}

/**
 * @brief Recursive function to find closest pair
 */
template<typename T>
T closestRecursive(std::vector<Point<T>*>& xPts,
std::vector<Point<T>*>& yPts, Edge<T>& closest) {
  if (xPts.size() <= 1) return std::numeric_limits<T>::max();

  size_t mid = xPts.size() / 2;
  T xMid = xPts[mid]->x;

  std::vector<Point<T>*> xL(xPts.begin(), xPts.begin() + mid);
  std::vector<Point<T>*> xR(xPts.begin() + mid, xPts.end());
  std::vector<Point<T>*> yL, yR;
  splitY(yPts, xMid, yL, yR);

  Edge<T> left, right;
  T dL = closestRecursive(xL, yL, left);
  T dR = closestRecursive(xR, yR, right);

  T delta = std::min(dL, dR);
  closest = (dL <= dR) ? left : right;

  std::vector<Point<T>*> strip;
  for (auto p : yPts) {
    if (std::abs(p->x - xMid) < delta) strip.push_back(p);
  }

  T dStrip = checkStrip(strip, delta, closest);
  return std::min(delta, dStrip);
}

/**
 * @brief Entry point to find closest pair among points
 * @param points input list of points
 * @param closestPair output closest pair edge
 * @return distance between closest pair
 */
template<typename T>
T ClosestPair(const std::vector<Point<T>>& points, Edge<T>& closestPair) {
  std::vector<Point<T>*> xPts, yPts;
  for (const auto& p : points) {
    xPts.push_back(const_cast<Point<T>*>(&p));
    yPts.push_back(const_cast<Point<T>*>(&p));
  }

  std::sort(xPts.begin(), xPts.end(), cmpX<T>);
  std::sort(yPts.begin(), yPts.end(), cmpY<T>);

  return closestRecursive(xPts, yPts, closestPair);
}
