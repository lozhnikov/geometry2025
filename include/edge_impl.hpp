/**
 * @file include/edge_impl.hpp
 * @author Mikhail Lozhnikov
 *
 * Реализация методов класса ребра.
 */

#ifndef INCLUDE_EDGE_IMPL_HPP_
#define INCLUDE_EDGE_IMPL_HPP_

#include <cmath>
#include <limits>
#include <utility>
#include <algorithm>
#include <point.hpp>
#include <edge.hpp>

namespace geometry {

template<typename T>
Edge<T>::Edge() :
  origin(Point(T(0.0), T(0.0))),
  destination(Point(T(1.0), T(0.0)))
{ }

template<typename T>
Edge<T>::Edge(const Point<T>& origin, const Point<T>& destination) :
  origin(origin),
  destination(destination)
{ }

template<typename T>
Edge<T>& Edge<T>::Rotate() {
  Point<T> middle = T(0.5) * (origin + destination);
  Point direction = destination - origin;
  Point normal(direction.Y(), -direction.X());

  origin = middle - T(0.5) * normal;
  destination = middle + T(0.5) * normal;

  return *this;
}

template<typename T>
Edge<T>& Edge<T>::Flip() {
  std::swap(origin, destination);

  return *this;
}

template<typename T>
Point<T> Edge<T>::Value(T t) const {
  return Point(origin + t * (destination - origin));
}

template<typename T>
Intersection Edge<T>::Intersect(const Edge& edge, T* t, T precision) const {
  Point<T> direction = destination - origin;
  Point<T> otherDirection = edge.destination - edge.origin;

  Point<T> otherNormal = Point(otherDirection.Y(),
      -otherDirection.X());

  T denominator = otherNormal * direction;

  if (std::fabs(denominator) < precision) {
    Position type = origin.Classify(edge, precision);

    if (type == Position::Left || type == Position::Right)
      return Intersection::Parallel;
    else
      return Intersection::Collinear;
  }

  T numerator = otherNormal * (origin - edge.origin);
  *t = - numerator / denominator;

  return Intersection::Skew;
}

template<typename T>
Intersection Edge<T>::Cross(const Edge& edge, T* t, T precision) {
  T s;
  Intersection crossType = edge.Intersect(*this, &s, precision);

  if (crossType == Intersection::Collinear ||
      crossType == Intersection::Parallel)
    return crossType;

  if (s < T(0) || s > T(1))
    return Intersection::SkewNoCross;

  Intersect(edge, t, precision);

  if (T(0) <= *t && *t <= T(1))
    return Intersection::SkewCross;

  return Intersection::SkewNoCross;
}

template<typename T>
bool Edge<T>::IsVertical(T precision) const {
  return std::fabs(origin.X() - destination.X()) <=
      precision * std::max(std::fabs(origin.X()), std::fabs(destination.X()));
}

template<typename T>
T Edge<T>::Slope(T precision) const {
  if (std::fabs(origin.X() - destination.X()) > precision *
      std::max(std::fabs(origin.X()), std::fabs(destination.X()))) {
    return (destination.Y() - origin.Y()) / (destination.X() - origin.X());
  }

  return std::numeric_limits<T>::max();
}

template<typename T>
T Edge<T>::Y(T x, T precision) const {
  return Slope(precision) * (x - origin.X()) + origin.Y();
}

}  // namespace geometry

#endif  // INCLUDE_EDGE_IMPL_HPP_
