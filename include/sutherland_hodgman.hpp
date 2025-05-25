/** 
 * @file include/sutherland_hodgman.hpp 
 * @brief Sutherland-Hodgman polygon clipping algorithm implementation.
 */

#ifndef INCLUDE_SUTHERLAND_HODGMAN_HPP_
#define INCLUDE_SUTHERLAND_HODGMAN_HPP_

#include <vector>
#include <algorithm>
#include <stdexcept>
#include "point.hpp"

namespace geometry {
namespace detail {

/**
 * @brief Inside test for clipping edge
 * @tparam T point coordinate type
 * @param p Test point
 * @param cp1 First point of clipping edge
 * @param cp2 Second point of clipping edge
 * @return true if point is inside the edge, false otherwise
 */
template<typename T>
bool Inside(const Point<T>& p, const Point<T>& cp1, const Point<T>& cp2) {
    return (cp2.X() - cp1.X()) * (p.Y() - cp1.Y()) > 
           (cp2.Y() - cp1.Y()) * (p.X() - cp1.X());
}

/**
 * @brief Compute intersection of two lines
 * @tparam T point coordinate type
 * @param cp1 First point of clipping edge
 * @param cp2 Second point of clipping edge
 * @param s Start point of subject edge
 * @param e End point of subject edge
 * @return Intersection point
 */
template<typename T>
Point<T> ComputeIntersection(const Point<T>& cp1, const Point<T>& cp2,
                            const Point<T>& s, const Point<T>& e) {
    Point<T> dc = {cp1.X() - cp2.X(), cp1.Y() - cp2.Y()};
    Point<T> dp = {s.X() - e.X(), s.Y() - e.Y()};
    T n1 = cp1.X() * cp2.Y() - cp1.Y() * cp2.X();
    T n2 = s.X() * e.Y() - s.Y() * e.X();
    T n3 = (dc.X() * dp.Y() - dc.Y() * dp.X());
    
    return {
        (n1 * dp.X() - n2 * dc.X()) / n3,
        (n1 * dp.Y() - n2 * dc.Y()) / n3
    };
}

} // namespace detail

/**
 * @brief Sutherland-Hodgman polygon clipping algorithm
 * @tparam T point coordinate type
 * @param subject_polygon Input polygon vertices
 * @param clip_polygon Convex clipping polygon vertices
 * @return std::vector<Point<T>> Clipped polygon vertices
 * @throws std::invalid_argument if either polygon has less than 3 vertices
 */
template<typename T>
std::vector<Point<T>> SutherlandHodgman(
    const std::vector<Point<T>>& subject_polygon,
    const std::vector<Point<T>>& clip_polygon) {
    
    if (subject_polygon.size() < 3 || clip_polygon.size() < 3) {
        throw std::invalid_argument("Polygons must have at least 3 vertices");
    }

    std::vector<Point<T>> output_list = subject_polygon;
    Point<T> cp1 = clip_polygon.back();
    
    for (const auto& cp2 : clip_polygon) {
        std::vector<Point<T>> input_list = output_list;
        output_list.clear();
        
        if (input_list.empty()) break;
        
        Point<T> s = input_list.back();
        for (const auto& e : input_list) {
            if (detail::Inside(e, cp1, cp2)) {
                if (!detail::Inside(s, cp1, cp2)) {
                    output_list.push_back(detail::ComputeIntersection(cp1, cp2, s, e));
                }
                output_list.push_back(e);
            } 
            else if (detail::Inside(s, cp1, cp2)) {
                output_list.push_back(detail::ComputeIntersection(cp1, cp2, s, e));
            }
            s = e;
        }
        cp1 = cp2;
    }
    
    return output_list;
}

} // namespace geometry

#endif  // INCLUDE_SUTHERLAND_HODGMAN_HPP_