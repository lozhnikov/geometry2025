/**
 * @file include/alg_graham.hpp
 * @author Almaz Sadikov
 *
 * @brief Implementation of the algorithm for constructing the Graham convex
 *  hull.
 */

#ifndef INCLUDE_ALG_GRAHAM_HPP_
#define INCLUDE_ALG_GRAHAM_HPP_

#include <vector>
#include <algorithm>
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
template <typename T>
T CrossProduct(const Point<T>& a, const Point<T>& b, const Point<T>& c) {
  return (b.X() - a.X()) * (c.Y() - a.Y()) -
         (b.Y() - a.Y()) * (c.X() - a.X());
}

/**
 * @brief Graham algorithm.
 *
 * @tparam T point coordinate type
 * @param points vector of points for processing
 * @return std::vector<Point<T>> convex hull points in traversal order
 */
template <typename T>
std::vector<Point<T>> AlgGraham(std::vector<Point<T>> points) {
  const size_t n = points.size();
  if (n <= 1) return points;

  std::sort(points.begin(), points.end());

  std::vector<Point<T>> hull;

  // Construct upper part of the hull
  for (size_t i = 0; i < n; ++i) {
    while (hull.size() >= 2 &&
           CrossProduct(hull[hull.size() - 2], hull.back(), points[i]) <= 0) {
      hull.pop_back();
    }
    hull.push_back(points[i]);
  }

  const size_t upper_hull_size = hull.size();
  // Construct lower part of the hull
  for (size_t i = n; i-- > 0;) {
    while (hull.size() > upper_hull_size &&
           CrossProduct(hull[hull.size() - 2], hull.back(), points[i]) <= 0) {
      hull.pop_back();
    }
    hull.push_back(points[i]);
  }

  if (!hull.empty()) {
    hull.pop_back();
  }

  return hull;
}
}  // namespace geometry

#endif  // INCLUDE_ALG_GRAHAM_HPP_