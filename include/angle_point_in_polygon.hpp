/** 
 * @file include/angle_point_in_polygon.hpp 
 * @author Yaroslav Kalugin
 * 
 * @brief Angle-based point-in-polygon algorithm implementation.
 */

#ifndef INCLUDE_ANGLE_POINT_IN_POLYGON_HPP_
#define INCLUDE_ANGLE_POINT_IN_POLYGON_HPP_

#include <vector>
#include <cmath>
#include "point.hpp"
#include "polygon.hpp"

namespace geometry {

/**
 * @brief Enumeration for point position relative to polygon
 */
enum class PointPosition {
    INSIDE,      ///< Point is inside the polygon
    OUTSIDE,     ///< Point is outside the polygon
    BOUNDARY     ///< Point is on the polygon boundary
};

/**
 * @brief Calculate the signed angle between vectors ab and ac
 * 
 * @tparam T point coordinate type
 * @param a first point
 * @param b second point
 * @param c third point
 * @param precision precision for calculations
 * @return T signed angle in degrees
 */
template<typename T>
double SignedAngle(const Point<T>& a, const Point<T>& b, const Point<T>& c, T precision) {
    Point<T> ab = b - a;
    Point<T> ac = c - a;
    
    double angle_ab = ab.PolarAngle(precision);
    double angle_ac = ac.PolarAngle(precision);
    
    if (angle_ab == -1.0 || angle_ac == -1.0) {
        return 180.0; // Collinear points
    }
    
    double angle_diff = angle_ac - angle_ab;
    
    if (angle_diff == 180.0 || angle_diff == -180.0) {
        return 180.0; // Collinear points
    }
    
    if (angle_diff < -180.0) {
        angle_diff += 360.0;
    } else if (angle_diff > 180.0) {
        angle_diff -= 360.0;
    }
    
    return angle_diff;
}

/**
 * @brief Determine if a point is inside, outside or on the boundary of a polygon using angle summation
 * 
 * @tparam T point coordinate type
 * @param point the point to test
 * @param polygon the polygon to test against
 * @param precision precision for calculations
 * @return PointPosition position of the point relative to polygon
 */
template<typename T>
PointPosition AnglePointInPolygon(const Point<T>& point, const Polygon<T>& polygon, T precision = T(1e-9)) {
    double total_angle = 0.0;
    
    // Создаем временную копию полигона для обхода
    Polygon<T> temp_polygon = polygon;
    
    for (size_t i = 0; i < temp_polygon.Size(); i++) {
        Edge<T> edge = temp_polygon.GetEdge();
        temp_polygon.Advance(Rotation::ClockWise);
        
        double angle = SignedAngle(point, edge.Origin(), edge.Destination(), precision);
        if (angle == 180.0) {
            return PointPosition::BOUNDARY;
        }
        total_angle += angle;
    }
    
    return (total_angle < -180.0) ? PointPosition::INSIDE : PointPosition::OUTSIDE;
}

}  // namespace geometry

#endif  // INCLUDE_ANGLE_POINT_IN_POLYGON_HPP_