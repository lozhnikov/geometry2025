/**
 * @file include/dimcirus.hpp
 * @author Dmitrii Chebanov
 *
 * @brief Cyrus-Beck line clipping algorithm implementation.
 */

#ifndef INCLUDE_DIMCIRUS_HPP_
#define INCLUDE_DIMCIRUS_HPP_

#include <algorithm>
#include <limits>
#include "common.hpp"
#include "edge.hpp"
#include "point.hpp"
#include "polygon.hpp"

namespace geometry {

/**
 * @brief Calculate the intersection of a line segment with a polygon edge.
 *
 * @tparam T numeric type for coordinates
 * @param edge The line segment to clip
 * @param polyEdge The polygon edge to test against
 * @param normal The normal vector to the polygon edge
 * @param[out] tEnter Entering intersection parameter (updated if found)
 * @param[out] tLeave Leaving intersection parameter (updated if found)
 * @return true if intersection is valid, false if segment is completely outside
 */
template<typename T>
bool calculateIntersection(const Edge<T>& edge, const Edge<T>& polyEdge,
                          const Point<T>& normal, T& tEnter, T& tLeave) {
    const Point<T>& A = edge.Origin();
    Point<T> dir = edge.Destination() - A;
    Point<T> toPoint = A - polyEdge.Origin();

    T denom = dir * normal;
    T numer = toPoint * normal;

    if (std::abs(denom) < std::numeric_limits<T>::epsilon()) {
        return (numer >= 0);
    }

    T t = -numer / denom;

    if (denom > 0) {
        if (t > tEnter) tEnter = t;
    } else {
        if (t < tLeave) tLeave = t;
    }

    return (tEnter <= tLeave);
}

/**
 * @brief Clip a line segment against a polygon using Cyrus-Beck algorithm.
 *
 * @tparam T numeric type for coordinates
 * @param edge The line segment to clip
 * @param poly The clipping polygon
 * @param[out] result The clipped segment (valid only if function returns true)
 * @return true if segment intersects polygon, false otherwise
 */
template<typename T>
bool clipLineSegment(const Edge<T>& edge, const Polygon<T>& poly, Edge<T>& result) {
    const Point<T>& A = edge.Origin();
    const Point<T>& B = edge.Destination();
    Point<T> dir = B - A;

    T tEnter = 0.0;
    T tLeave = 1.0;

    for (auto it = poly.Vertices().begin(); it != poly.Vertices().end(); ++it) {
        auto next = std::next(it);
        if (next == poly.Vertices().end()) {
            next = poly.Vertices().begin();
        }

        Edge<T> polyEdge(*it, *next);
        Point<T> normal = polyEdge.Rotate().Destination() - polyEdge.Rotate().Origin();

        if (!calculateIntersection(edge, polyEdge, normal, tEnter, tLeave)) {
            return false;
        }
    }

    if (tEnter <= tLeave) {
        result = Edge<T>(A + dir * tEnter, A + dir * tLeave);
        return true;
    }

    return false;
}

}  // namespace geometry

#endif  // INCLUDE_DIMCIRUS_HPP_
