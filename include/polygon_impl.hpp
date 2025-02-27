/**
 * @file include/polygon_impl.hpp
 * @author Mikhail Lozhnikov
 *
 * Реализация методов класса многоугольника.
 */

#ifndef INCLUDE_POLYGON_IMPL_HPP_
#define INCLUDE_POLYGON_IMPL_HPP_

#include <utility>
#include <point.hpp>
#include <edge.hpp>
#include <polygon.hpp>

namespace geometry {

template<typename T, typename Container>
Polygon<T, Container>::Polygon() :
  vertices(),
  current(vertices.begin())
{ }

template<typename T, typename Container>
Polygon<T, Container>::Polygon(const Container& vertices) :
  vertices(vertices),
  current(this->vertices.begin())
{ }

template<typename T, typename Container>
Polygon<T, Container>::Polygon(const Container& vertices,
        typename Container::iterator position) :
  vertices(vertices),
  current(std::next(this->vertices.begin(),
      std::distance(vertices.begin(),
                    static_cast<typename Container::const_iterator>(position))))
{ }

template<typename T, typename Container>
Polygon<T, Container>::Polygon(const Polygon& other) :
  vertices(other.vertices),
  current(std::next(vertices.begin(),
      std::distance(other.vertices.begin(),
          static_cast<typename Container::const_iterator>(other.current))))
{ }

template<typename T, typename Container>
Polygon<T, Container>::Polygon(Polygon&& other) :
  vertices(),
  current() {
  std::ptrdiff_t distance = std::distance(other.vertices.begin(),
      other.current);

  vertices = std::move(other.vertices);
  current = std::next(vertices.begin(), distance);
}

template<typename T, typename Container>
Polygon<T, Container>& Polygon<T, Container>::operator=(const Polygon& other) {
  vertices = other.vertices;
  current = std::next(vertices.begin(),
      std::distance(other.vertices.begin(),
      static_cast<typename Container::const_iterator>(other.current)));

  return *this;
}

template<typename T, typename Container>
Polygon<T, Container>& Polygon<T, Container>::operator=(Polygon&& other) {
  std::ptrdiff_t distance = std::distance(other.vertices.begin(),
      other.current);

  vertices = std::move(other.vertices);
  current = std::next(vertices.begin(), distance);

  return *this;
}

template<typename T, typename Container>
Edge<T> Polygon<T, Container>::GetEdge() const {
  return Edge<T>(*current, *ClockWise());
}

template<typename T, typename Container>
typename Container::iterator Polygon<T, Container>::ClockWise() {
  if (vertices.empty())
    return vertices.end();

  if (current == vertices.end())
    return vertices.begin();

  if (std::next(current) != vertices.end())
    return std::next(current);

  return vertices.begin();
}

template<typename T, typename Container>
typename Container::iterator Polygon<T, Container>::CounterClockWise() {
  if (vertices.empty())
    return vertices.end();

  if (current != vertices.begin())
    return std::prev(current);

  return std::prev(vertices.end());
}

template<typename T, typename Container>
typename Container::const_iterator Polygon<T, Container>::ClockWise() const {
  if (vertices.empty())
    return vertices.end();

  if (current == vertices.end())
    return vertices.begin();

  if (std::next(current) != vertices.end())
    return std::next(current);

  return vertices.begin();
}

template<typename T, typename Container>
typename Container::const_iterator
Polygon<T, Container>::CounterClockWise() const {
  if (vertices.empty())
    return vertices.end();

  if (current != vertices.begin())
    return std::prev(current);

  return std::prev(vertices.end());
}

template<typename T, typename Container>
typename Container::iterator
Polygon<T, Container>::Neighbor(Rotation rotation) {
  switch (rotation) {
  case Rotation::ClockWise:
    return ClockWise();
  case Rotation::CounterClockWise:
    return CounterClockWise();
  default:
    return vertices.end();
  }
}

template<typename T, typename Container>
typename Container::const_iterator
Polygon<T, Container>::Neighbor(Rotation rotation) const {
  switch (rotation) {
  case Rotation::ClockWise:
    return ClockWise();
  case Rotation::CounterClockWise:
    return CounterClockWise();
  default:
    return vertices.end();
  }
}

template<typename T, typename Container>
typename Container::iterator
Polygon<T, Container>::Advance(Rotation rotation) {
  current = Neighbor(rotation);

  return current;
}

template<typename T, typename Container>
typename Container::iterator
Polygon<T, Container>::Insert(const Point<T>& point) {
  if (current != vertices.end())
    current = std::next(current);

  current = vertices.insert(current, point);

  return current;
}

template<typename T, typename Container>
void Polygon<T, Container>::Remove(typename Container::iterator position) {
  current = vertices.erase(position);

  Advance(Rotation::CounterClockWise);
}

template<typename T, typename Container>
Polygon<T, Container> Polygon<T, Container>::Split(
    typename Container::iterator position) {
  Container other;

  other.push_back(*current);

  if (current == position) {
    return Polygon(other, std::prev(other.end()));
  }

  typename Container::iterator it = std::next(current);

  while (it != position) {
    if (it == vertices.end()) {
      it = vertices.begin();
      continue;
    }

    other.push_back(*it);
    it = vertices.erase(it);
  }

  other.push_back(*it);

  return Polygon(other, std::prev(other.end()));
}

}  // namespace geometry

#endif  // INCLUDE_POLYGON_IMPL_HPP_
