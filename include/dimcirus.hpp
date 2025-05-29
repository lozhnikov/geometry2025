/**
 * @file include/dimcirus.hpp
 * @author Dmitrii Chebanov
 *
 */

#ifndef INCLUDE_DIMCIRUS_HPP
#define INCLUDE_DIMCIRUS_HPP


#include <common.hpp>
#include <edge.hpp>
#include <polygon.hpp>

namespace geometry {

template<typename T>
bool clipLineSegment(Edge<T>& s, Polygon<T>& p, Edge<T>& result) {
    T t0 = 0.0;
    T t1 = 1.0;
    Point<T> v = s.Destination() - s.Origin();

    for (size_t i = 0; i < p.Size(); ++i) {
        Edge<T> e = p.GetEdge();
        T t;
        Intersection intersection = s.Intersect(e, &t, T(1e-6));

        if (intersection == Intersection::Skew) {
            Edge<T> f = e;
            f.Rotate();
            Point<T> n = f.Destination() - f.Origin();

            T dotProduct = n.X() * v.X() + n.Y() * v.Y();

            if (dotProduct > T(0)) {
                t0 = std::max(t0, t);
            } else { 
                t1 = std::min(t1, t);
            }
        } else if (intersection == Intersection::Parallel || intersection == Intersection::Collinear) {
            Position pos = s.Origin().Classify(e, T(1e-6));
            if (pos == Position::Left) {
                return false;
            }
        }

        p.Advance(Rotation::ClockWise);
    }

    if (t0 <= t1) {
        result = Edge<T>(s.Value(t0), s.Value(t1));
        return true;
    }

    return false;
}

} 

#endif // INCLUDE_DIMCIRUS_HPP
