/**
 * @file include/point_impl.hpp
 * @author Mikhail Lozhnikov
 *
 * Реализация некоторых методов класса точки.
 */

#ifndef INCLUDE_POINT_IMPL_HPP_
#define INCLUDE_POINT_IMPL_HPP_

#include <point.hpp>
#include <edge.hpp>

namespace geometry {

template<typename T>
Position Point<T>::Classify(const Edge<T>& edge, T precision) const {
  return Classify(edge.Origin(), edge.Destination(), precision);
}

template<typename T>
T Point<T>::Distance(const Edge<T>& edge, T precision) const {
  Edge<T> rotated = edge;

  rotated.Flip().Rotate();

  Point<T> normal(rotated.Destination() - rotated.Origin());

  normal = (T(1.0) / normal.Length()) * normal;

  Edge normalEdge(*this, *this + normal);

  T t = T();

  normalEdge.Intersect(edge, &t, precision);

  return t;
}

}  // namespace geometry

#endif  // INCLUDE_POINT_IMPL_HPP_

