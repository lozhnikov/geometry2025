/**
 * @file include/clip_algorithm.hpp
 * @author Dmitrii Chebanov
 *
 */

#ifndef INCLUDE_CLIP_ALGORITHM_HPP_
#define INCLUDE_CLIP_ALGORITHM_HPP_

#include <edge.hpp>
#include <polygon.hpp>
#include <point.hpp>
#include <common.hpp>
#include <limits>
#include <vector>

namespace geometry {

/**
 * @brief Алгоритм отсечения отрезка многоугольником (Цирус-Бек)
 *
 * @tparam T
 * @param edge
 * @param poly
 * @param result
 * @return
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

        Point<T> edgeVec = polyEdge.Destination() - polyEdge.Origin();
        Point<T> toPoint = A - polyEdge.Origin();

        T denom = dir * normal;
        T numer = toPoint * normal;

        if (std::abs(denom) < std::numeric_limits<T>::epsilon()) {
            if (numer < 0) {
                return false;
            }
            continue;
        }

        T t = -numer / denom;

        if (denom > 0) {
            if (t > tEnter) {
                tEnter = t;
            }
        } else {
            if (t < tLeave) {
                tLeave = t;
            }
        }

        if (tEnter > tLeave) {
            return false;
        }
    }

    if (tEnter <= tLeave) {
        result = Edge<T>(A + dir * tEnter, A + dir * tLeave);
        return true;
    }

    return false;
}

}

#endif // INCLUDE_CLIP_ALGORITHM_HPP_
