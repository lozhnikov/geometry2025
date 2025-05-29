/**
 * @file include/check_convex.hpp
 * @author Daria Slepenkova
 *
 * @brief Implementation of an algorithm for checking the convexity of a polygon vertex.
 */

#ifndef INCLUDE_CHECK_CONVEX_HPP_
#define INCLUDE_CHECK_CONVEX_HPP_

#include <vector>
#include <list>
#include <cmath>
#include <iostream>
#include "polygon.hpp"
#include "point.hpp"
#include "common.hpp"

namespace geometry {

/**
 * @brief Calculating the square of the length.
 *
 * @tparam T point coordinate type
 * @param a point
 * @return T square of the length
 */
  template<typename T>
  T Length2(const Point<T>& a) {
    return (a.X()*a.X() + a.Y()*a.Y());
  }

   /**
   * @brief Algorithm for checking the convexity.
   *
   * @tparam T point coordinate type
   * @param points vector of points for processing
   * @return std::vector<Point<T>> convex vertices of a polygon in traversal order
   */
  template<typename T>
  std::vector<Point<T>> CheckConvex(std::vector<Point<T>> points) {
    std::vector<Point<T>> convex;
    Polygon<T> polygon(std::list<Point<T>>(points.begin(), points.end()));

    if (polygon.Size() < 3) {
        return convex;
    }

    auto start = polygon.Current();
    auto current = start;

    do {
        auto u = polygon.CounterClockWise();
        auto w = polygon.ClockWise();

        Point<T> a = (*current) - (*u);
        Point<T> b = (*w) - (*u);
        T sa = a.X() * b.Y() - b.X() * a.Y();

        if (sa < 0.0 || (std::abs(sa) < 1e-10 && Length2(a) < Length2(b))) {
            convex.push_back(*current);
        }

        polygon.Advance(Rotation::ClockWise);
        current = polygon.Current();

    } while (current != start);

     return convex;
    }
}  // namespace geometry

#endif  // INCLUDE_CHECK_CONVEX_HPP_






