#ifndef INCLUDE_SUTHERLAND_HODGMAN_HPP_
#define INCLUDE_SUTHERLAND_HODGMAN_HPP_

#include <vector>
#include "point.hpp"

namespace geometry {

template<typename T>
bool Inside(const Point<T>& a, const Point<T>& b, const Point<T>& p) {
  return (b.X() - a.X()) * (p.Y() - a.Y()) - (b.Y() - a.Y()) * (p.X() - a.X()) >= 0;
}

template<typename T>
Point<T> Intersect(const Point<T>& a, const Point<T>& b,
                   const Point<T>& p, const Point<T>& q) {
  T A1 = b.Y() - a.Y();
  T B1 = a.X() - b.X();
  T C1 = A1 * a.X() + B1 * a.Y();

  T A2 = q.Y() - p.Y();
  T B2 = p.X() - q.X();
  T C2 = A2 * p.X() + B2 * p.Y();

  T det = A1 * B2 - A2 * B1;
  return Point<T>((B2 * C1 - B1 * C2) / det,
                  (A1 * C2 - A2 * C1) / det);
}

template<typename T>
std::vector<Point<T>> SutherlandHodgman(const std::vector<Point<T>>& subject,
                                        const std::vector<Point<T>>& clip) {
  std::vector<Point<T>> output = subject;

  for (size_t i = 0; i < clip.size(); ++i) {
    std::vector<Point<T>> input = output;
    output.clear();

    Point<T> A = clip[i];
    Point<T> B = clip[(i + 1) % clip.size()];

    for (size_t j = 0; j < input.size(); ++j) {
      Point<T> P = input[j];
      Point<T> Q = input[(j + 1) % input.size()];

      bool P_inside = Inside(A, B, P);
      bool Q_inside = Inside(A, B, Q);

      if (P_inside && Q_inside) {
        output.push_back(Q);
      } else if (P_inside && !Q_inside) {
        output.push_back(Intersect(A, B, P, Q));
      } else if (!P_inside && Q_inside) {
        output.push_back(Intersect(A, B, P, Q));
        output.push_back(Q);
      }
    }
  }

  return output;
}

}  // namespace geometry

#endif  // INCLUDE_SUTHERLAND_HODGMAN_HPP_