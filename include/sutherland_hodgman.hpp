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
            const auto cp2x_cp1x = cp2.X() - cp1.X();
            const auto cp2y_cp1y = cp2.Y() - cp1.Y();
            const auto e_inside = cp2x_cp1x * (e.Y() - cp1.Y()) > cp2y_cp1y * (e.X() - cp1.X());
            const auto s_inside = cp2x_cp1x * (s.Y() - cp1.Y()) > cp2y_cp1y * (s.X() - cp1.X());
            
            if (e_inside) {
                if (!s_inside) {
                    const auto dcx = cp1.X() - cp2.X();
                    const auto dcy = cp1.Y() - cp2.Y();
                    const auto dpx = s.X() - e.X();
                    const auto dpy = s.Y() - e.Y();
                    const auto n3 = dcx * dpy - dcy * dpx;
                    
                    if (n3 != 0) { 
                        const auto n1 = cp1.X() * cp2.Y() - cp1.Y() * cp2.X();
                        const auto n2 = s.X() * e.Y() - s.Y() * e.X();
                        output_list.emplace_back(
                            (n1 * dpx - n2 * dcx) / n3,
                            (n1 * dpy - n2 * dcy) / n3
                        );
                    }
                }
                output_list.push_back(e);
            } 
            else if (s_inside) {
                const auto dcx = cp1.X() - cp2.X();
                const auto dcy = cp1.Y() - cp2.Y();
                const auto dpx = s.X() - e.X();
                const auto dpy = s.Y() - e.Y();
                const auto n3 = dcx * dpy - dcy * dpx;
                
                if (n3 != 0) {
                    const auto n1 = cp1.X() * cp2.Y() - cp1.Y() * cp2.X();
                    const auto n2 = s.X() * e.Y() - s.Y() * e.X();
                    output_list.emplace_back(
                        (n1 * dpx - n2 * dcx) / n3,
                        (n1 * dpy - n2 * dcy) / n3
                    );
                }
            }
            s = e;
        }
        cp1 = cp2;
    }
    
    return output_list;
}

} // namespace geometry

#endif  // INCLUDE_SUTHERLAND_HODGMAN_HPP_
