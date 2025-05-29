/**
 * @file include/cyrus_bek.hpp
 * 
 * @brief Cyrus-Bek line clipping algorithm implementation.
 */

#ifndef INCLUDE_CYRUS_BEK_HPP_
#define INCLUDE_CYRUS_BEK_HPP_

#include <vector>
#include <algorithm>
#include "point.hpp"
#include "edge.hpp"
#include "polygon.hpp"

namespace geometry {

/**
 * @brief Clip a line segment against a convex polygon using Cyrus-Bek algorithm.
 *
 * @tparam T coordinate type
 * @param s line segment to clip
 * @param p convex clipping polygon
 * @param result clipped segment (output)
 * @return true if segment is visible after clipping
 * @return false if segment is completely outside
 */
template<typename T>
bool ClipLineSegment(const Edge<T>& s, const Polygon<T>& p, Edge<T>& result) {
    double t0 = 0.0;
    double t1 = 1.0;
    Point<T> v = s.Dest() - s.Org();

    for (size_t i = 0; i < p.Size(); i++) {
        Edge<T> e = p.Edge(i);
        double t;
        IntersectionType intersect = s.Intersect(e, t);

        if (intersect == SKEW) {
            Edge<T> f = e;
            f.Rotate();
            Point<T> n = f.Dest() - f.Org();
            
            if (DotProduct(n, v) > 0.0) { // Entering point
                if (t > t0) t0 = t;
            } else { // Leaving point
                if (t < t1) t1 = t;
            }
        } else if (intersect == PARALLEL) {
            if (s.Org().Classify(e) == LEFT) {
                return false;
            }
        }

        if (t0 > t1) return false;
    }

    if (t0 <= t1) {
        result = Edge<T>(s.Point(t0), s.Point(t1));
        return true;
    }
    return false;
}

} // namespace geometry

#endif // INCLUDE_CYRUS_BEK_HPP_
