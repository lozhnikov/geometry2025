/**
 * @file include/common.hpp
 * @author Mikhail Lozhnikov
 *
 * Некоторые общие константы и структуры.
 */

#ifndef INCLUDE_COMMON_HPP_
#define INCLUDE_COMMON_HPP_

namespace geometry {

#define PI 3.14159265358979323846

/**
 * Положение точки относительно отрезка.
 */
enum class Position {
  Left,
  Right,
  Beyond,
  Behind,
  Between,
  Origin,
  Destination
};

/**
 * Расположение двух отрезков.
 */
enum class Intersection {
  Collinear,
  Parallel,
  Skew,
  SkewCross,
  SkewNoCross
};

/**
 * Направление вращения.
 */
enum class Rotation {
  ClockWise,
  CounterClockWise
};

}  // namespace geometry

#endif  // INCLUDE_COMMON_HPP_
