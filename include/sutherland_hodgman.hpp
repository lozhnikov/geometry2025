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
 * @brief Implements the Sutherland-Hodgman polygon clipping algorithm.
 *
 * Clips a subject polygon against a convex clipping polygon.
 *
 * @tparam T Numeric type for point coordinates
 * @param subject_polygon Vertices of the polygon to be clipped
 * @param clip_polygon Vertices of the convex clipping polygon
 * @return std::vector<Point<T>> Vertices of the resulting clipped polygon
 * @throws std::invalid_argument if either polygon has fewer than 3 vertices
 */
template <typename T>
std::vector<Point<T>> SutherlandHodgmanClip(
    const std::vector<Point<T>>& subject_polygon,
    const std::vector<Point<T>>& clip_polygon) {
  if (subject_polygon.size() < 3 || clip_polygon.size() < 3) {
    throw std::invalid_argument("Polygons must have at least 3 vertices");
  }

  std::vector<Point<T>> output_list = subject_polygon;
  Point<T> clip_point1 = clip_polygon.back();
  
  for (const auto& clip_point2 : clip_polygon) {
    std::vector<Point<T>> input_list = output_list;
    output_list.clear();
    
    if (input_list.empty()) break;
    
    Point<T> subject_point = input_list.back();
    for (const auto& edge_point : input_list) {
      const auto clip_dx = clip_point2.x() - clip_point1.x();
      const auto clip_dy = clip_point2.y() - clip_point1.y();
      
      const bool is_edge_inside = (clip_dx * (edge_point.y() - clip_point1.y()) > 
                                  clip_dy * (edge_point.x() - clip_point1.x()));
      const bool is_subject_inside = (clip_dx * (subject_point.y() - clip_point1.y()) > 
                                     clip_dy * (subject_point.x() - clip_point1.x()));
      
      if (is_edge_inside) {
        if (!is_subject_inside) {
          const auto edge_dx = subject_point.x() - edge_point.x();
          const auto edge_dy = subject_point.y() - edge_point.y();
          const auto denominator = (clip_point1.x() - clip_point2.x()) * edge_dy - 
                                  (clip_point1.y() - clip_point2.y()) * edge_dx;
          
          if (denominator != 0) { 
            const auto clip_numerator = clip_point1.x() * clip_point2.y() - 
                                      clip_point1.y() * clip_point2.x();
            const auto edge_numerator = subject_point.x() * edge_point.y() - 
                                      subject_point.y() * edge_point.x();
            output_list.emplace_back(
                (clip_numerator * edge_dx - edge_numerator * (clip_point1.x() - clip_point2.x())) / denominator,
                (clip_numerator * edge_dy - edge_numerator * (clip_point1.y() - clip_point2.y())) / denominator);
          }
        }
        output_list.push_back(edge_point);
      } else if (is_subject_inside) {
        const auto edge_dx = subject_point.x() - edge_point.x();
        const auto edge_dy = subject_point.y() - edge_point.y();
        const auto denominator = (clip_point1.x() - clip_point2.x()) * edge_dy - 
                                (clip_point1.y() - clip_point2.y()) * edge_dx;
        
        if (denominator != 0) {
          const auto clip_numerator = clip_point1.x() * clip_point2.y() - 
                                    clip_point1.y() * clip_point2.x();
          const auto edge_numerator = subject_point.x() * edge_point.y() - 
                                    subject_point.y() * edge_point.x();
          output_list.emplace_back(
              (clip_numerator * edge_dx - edge_numerator * (clip_point1.x() - clip_point2.x())) / denominator,
              (clip_numerator * edge_dy - edge_numerator * (clip_point1.y() - clip_point2.y())) / denominator);
        }
      }
      subject_point = edge_point;
    }
    clip_point1 = clip_point2;
  }
  
  return output_list;
}

}  // namespace geometry

#endif  // INCLUDE_SUTHERLAND_HODGMAN_HPP_
