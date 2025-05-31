/**
 * @file include/cyrus_beck.hpp
 * 
 * @brief Cyrus-Beck line clipping algorithm implementation.
 */

#ifndef INCLUDE_CYRUS_BECK_HPP_
#define INCLUDE_CYRUS_BECK_HPP_

#include <vector>
#include <iostream>
#include <algorithm>
#include "point.hpp"
#include "edge.hpp"
#include "polygon.hpp"

namespace geometry {

/**
 * @brief Compute the dot product of two points.
 *
 * @tparam T coordinate type
 * @param a first point
 * @param b second point
 * @return The dot product of the two points
 */
template<typename T>
T DotProduct(const Point<T>& a, const Point<T>& b) {
    return a.X() * b.X() + a.Y() * b.Y();
}

/**
 * @brief Clip a line segment against a convex polygon using Cyrus-Beck algorithm.
 *
 * @tparam T coordinate type
 * @param s line segment to clip
 * @param p convex clipping polygon
 * @param result clipped segment (output)
 * @return true if segment is visible after clipping
 * @return false if segment is completely outside
 */
template<typename T>
bool ClipLineSegment(const Edge<T>& s, Polygon<T>* p, Edge<T>* result) {
    T t0 = 0.0;
    T t1 = 1.0;
    T t;
    Point<T> v = s.Destination() - s.Origin();

    for (int i = 0; i < p->Size(); i++, p->Advance(Rotation::ClockWise)) {
        Edge<T> e = p->GetEdge();
        Intersection intersect = s.Intersect(e, &t, 1e-10);

        if (intersect == Intersection::Skew) {
            Edge<T> f = e;
            f.Rotate();
            Point<T> n = f.Destination() - f.Origin();
            if (DotProduct(n, v) > 0.0) {  // Entering point
                if (t > t0) t0 = t;
            } else {  // Leaving point
                if (t < t1) t1 = t;
            }
        } else {
            if (s.Origin().Classify(e, 1e-10) == Position::Left) {
                return false;
            }
        }
    }

    if (t0 <= t1) {
        *result = Edge<T>(s.Value(t0), s.Value(t1));
        return true;
    }
    return false;
}
}  // namespace geometry

#endif  // INCLUDE_CYRUS_BECK_HPP_
