// include/gift_wrapping.hpp
/**
 * @file include/gift_wrapping.hpp
 * @author Ivan Kuznetsov
 *
 * @brief Gift wrapping convex hull algorithm implementation.
 */

#ifndef INCLUDE_GIFT_WRAPPING_HPP_
#define INCLUDE_GIFT_WRAPPING_HPP_

#include <vector>
#include "point.hpp"

namespace geometry {

/**
 * @brief Calculate the cross product to determine orientation.
 *
 * @tparam T point coordinate type
 * @param a first point
 * @param b second point
 * @param c third point
 * @return T cross product value
 */
template<typename T>
T CrossProduct(const Point<T>& a, const Point<T>& b, const Point<T>& c) {
  return (b.X() - a.X()) * (c.Y() - a.Y())
         - (b.Y() - a.Y()) * (c.X() - a.X());
}

/**
 * @brief Gift wrapping algorithm.
 *
 * @tparam T point coordinate type
 * @param points vector of points for processing
 * @return std::vector<Point<T>> convex hull points in traversal order
 */
template<typename T>
std::vector<Point<T>> GiftWrapping(std::vector<Point<T>> points) {
  size_t n = points.size();
  if (n <= 2) return points;

  // Find leftmost point
  size_t leftmost = 0;
  for (size_t i = 1; i < n; i++) {
    if (points[i].X() < points[leftmost].X() ||
        (points[i].X() == points[leftmost].X() &&
         points[i].Y() < points[leftmost].Y())) {
      leftmost = i;
    }
  }

  std::vector<Point<T>> hull;
  size_t current = leftmost;
  size_t next;

  do {
    hull.push_back(points[current]);
    next = (current + 1) % n;

    for (size_t i = 0; i < n; i++) {
      if (i == current) continue;
      T cross = CrossProduct(points[current], points[next], points[i]);
      if (cross < 0) {
        next = i;
      } else if (cross == 0) {
        // For collinear points, select farthest
        T dist_next = (points[next].X() - points[current].X()) *
                      (points[next].X() - points[current].X()) +
                      (points[next].Y() - points[current].Y()) *
                      (points[next].Y() - points[current].Y());
        T dist_i = (points[i].X() - points[current].X()) *
                   (points[i].X() - points[current].X()) +
                   (points[i].Y() - points[current].Y()) *
                   (points[i].Y() - points[current].Y());
        if (dist_i > dist_next) {
          next = i;
        }
      }
    }
    current = next;
  } while (current != leftmost);

  return hull;
}

}  // namespace geometry

#endif  // INCLUDE_GIFT_WRAPPING_HPP_
