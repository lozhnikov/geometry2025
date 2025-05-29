/**
 * @file include/convex_intersection.hpp
 * @author German Semenov
 *
 * @brief Convex intersection algorithm.
 */

#ifndef INCLUDE_CONVEX_INTERSECTION_HPP_
#define INCLUDE_CONVEX_INTERSECTION_HPP_

#include <vector>
#include "point.hpp"

namespace geometry {

template <typename T>
bool IsInsideConvex(const std::vector<Point<T>>& polygon,
                    const Point<T>& p) {
  size_t n = polygon.size();
  for (size_t i = 0; i < n; ++i) {
    Point<T> a = polygon[i];
    Point<T> b = polygon[(i + 1) % n];
    T cross = (b.X() - a.X()) * (p.Y() - a.Y()) -
              (b.Y() - a.Y()) * (p.X() - a.X());
    if (cross < 0) return false;
  }
  return true;
}

template <typename T>
Point<T> ComputeIntersection(const Point<T>& a,
                             const Point<T>& b,
                             const Point<T>& c,
                             const Point<T>& d) {
  T A1 = b.Y() - a.Y();
  T B1 = a.X() - b.X();
  T C1 = A1 * a.X() + B1 * a.Y();

  T A2 = d.Y() - c.Y();
  T B2 = c.X() - d.X();
  T C2 = A2 * c.X() + B2 * c.Y();

  T det = A1 * B2 - A2 * B1;
  return Point<T>((B2 * C1 - B1 * C2) / det,
                  (A1 * C2 - A2 * C1) / det);
}

template <typename T>
std::vector<Point<T>> ConvexIntersection(
    const std::vector<Point<T>>& A,
    const std::vector<Point<T>>& B) {
  std::vector<Point<T>> result;

  for (const auto& p : A) {
    if (IsInsideConvex(B, p)) result.push_back(p);
  }

  for (const auto& p : B) {
    if (IsInsideConvex(A, p)) result.push_back(p);
  }

  for (size_t i = 0; i < A.size(); ++i) {
    Point<T> a1 = A[i];
    Point<T> a2 = A[(i + 1) % A.size()];

    for (size_t j = 0; j < B.size(); ++j) {
      Point<T> b1 = B[j];
      Point<T> b2 = B[(j + 1) % B.size()];

      T d1 = (a2.X() - a1.X()) * (b1.Y() - a1.Y()) -
             (a2.Y() - a1.Y()) * (b1.X() - a1.X());
      T d2 = (a2.X() - a1.X()) * (b2.Y() - a1.Y()) -
             (a2.Y() - a1.Y()) * (b2.X() - a1.X());
      T d3 = (b2.X() - b1.X()) * (a1.Y() - b1.Y()) -
             (b2.Y() - b1.Y()) * (a1.X() - b1.X());
      T d4 = (b2.X() - b1.X()) * (a2.Y() - b1.Y()) -
             (b2.Y() - b1.Y()) * (a2.X() - b1.X());

      if (d1 * d2 < 0 && d3 * d4 < 0) {
        result.push_back(
            ComputeIntersection(a1, a2, b1, b2));
      }
    }
  }

  return result;
}

}  // namespace geometry

#endif  // INCLUDE_CONVEX_INTERSECTION_HPP_
