/**
 * @file include/convex_hull_graham.hpp 
 * @author Almaz Sadikov
 * 
 * @brief implementation of the algorithm for constructing a convex
 *  hull by the Graham method
 */

#ifndef INCLUDE_CONVEX_HULL_GRAHAM_HPP_
#define INCLUDE_CONVEX_HULL_GRAHAM_HPP_


#include <vector>
#include <algorithm>
#include "point.hpp"

namespace geometry {

/**
 * @brief Custom polar angle comparator for point sorting
 */
template<typename T>
struct PolarAngleComparator {
  Point<T> pivot;
    
  explicit PolarAngleComparator(const Point<T>& p) : pivot(p) {}
    
  bool operator()(const Point<T>& a, const Point<T>& b) const {
    // Compute orientation
    double orientation = (a.X() - pivot.X()) * (b.Y() - pivot.Y()) 
             - (a.Y() - pivot.Y()) * (b.X() - pivot.X());
        
    if (orientation == 0) {
      // If collinear, get the farthest point by comparing squared distances
      double dist_a = (a.X() - pivot.X())*(a.X() - pivot.X()) + 
                      (a.Y() - pivot.Y())*(a.Y() - pivot.Y());
      double dist_b = (b.X() - pivot.X())*(b.X() - pivot.X()) + 
                      (b.Y() - pivot.Y())*(b.Y() - pivot.Y());
      return dist_a < dist_b;
    }
    return orientation > 0;
  }
};

/**
 * @brief Alternative Graham scan implementation with different point
 *  processing
 */
template<typename T>
std::vector<Point<T>> ComputeConvexHull(std::vector<Point<T>> points) {
  if (points.size() <= 2) return points;

  // Find the pivot point (lowest y, then leftmost)
  auto pivot_it = std::min_element(points.begin(), points.end(),
     [](const Point<T>& a, const Point<T>& b) {
      return (a.Y() < b.Y()) || (a.Y() == b.Y() && a.X() < b.X());
    });
    
  Point<T> pivot = *pivot_it;
  points.erase(pivot_it);

  // Sort by polar angle with custom comparator
  std::sort(points.begin(), points.end(), PolarAngleComparator<T>(pivot));

  // Prepare the convex hull
  std::vector<Point<T>> hull;
  hull.push_back(pivot);
  hull.push_back(points[0]);
    
  // Build the hull with different point processing
  for (size_t i = 1; i < points.size(); ) {
    Point<T> top = hull.back();
    hull.pop_back();
        
    if (hull.empty()) {
      hull.push_back(top);
      hull.push_back(points[i]);
      i++;
      continue;
    }
        
    Point<T> next_to_top = hull.back();
    double cross = (top.X() - next_to_top.X()) * (points[i].Y()
    - next_to_top.Y()) 
          - (top.Y() - next_to_top.Y()) * (points[i].X() - next_to_top.X());
        
    if (cross > 0) {
      hull.push_back(top);
      hull.push_back(points[i]);
      i++;
    }
  }

  return hull;
}

}  // namespace geometry
#endif  // INCLUDE_CONVEX_HULL_GRAHAM_HPP_
