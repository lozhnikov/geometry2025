/**
 * @file include/my_polig.hpp
 * @author Nika Adzhindzhal
 *
 */

#ifndef INCLUDE_MY_POLIG_HPP_
#define INCLUDE_MY_POLIG_HPP_

#include <vector>
#include <memory>
#include <list>
#include <point.hpp>
#include <polygon.hpp>

namespace geometry {

    template<typename T>
    int polarCmp(const Point<T>& p, const Point<T>& q,
        const Point<T>& originPt, T precision) {
        Point<T> vp = p - originPt;
        Point<T> vq = q - originPt;

        T angle_p = vp.PolarAngle(precision);
        T angle_q = vq.PolarAngle(precision);

        if (angle_p < angle_q) return -1;
        if (angle_p > angle_q) return 1;

        T len_p = vp.Length();
        T len_q = vq.Length();

        if (len_p < len_q) return -1;
        if (len_p > len_q) return 1;
        return 0;
    }

    template<typename T>
    Polygon<T>* starPolygon(const std::vector<Point<T>>& points, T precision) {
        if (points.empty()) return nullptr;

        Polygon<T>* polygon = new Polygon<T>();
        polygon->Insert(points[0]);
        std::list<Point<T>> origin = polygon->Vertices();
        Point<T> originPt = origin.front();

        auto itToOrigin = polygon->Current();

        for (size_t i = 1; i < points.size(); ++i) {
            polygon->Current() = itToOrigin;
            polygon->Advance(Rotation::ClockWise);

            while (polarCmp(points[i], *polygon->Current(),
                originPt, precision) < 0) {
                polygon->Advance(Rotation::ClockWise);
            }

            polygon->Advance(Rotation::CounterClockWise);
            polygon->Insert(points[i]);
        }

        return polygon;
    }

}  // namespace geometry

#endif  // INCLUDE_MY_POLIG_HPP_
