/**
 * @file include/orientation.hpp
 * @author Mikhail Lozhnikov
 *
 * Реализация функции определения ориентации трёх точек.
 */

#ifndef INCLUDE_ORIENTATION_HPP_
#define INCLUDE_ORIENTATION_HPP_

#include <point.hpp>

namespace geometry {

/**
 * @brief Вычислить ориентацию трёх точек.
 *
 * @tparam ValueType Тип данных, используемый для вычислений.
 *
 * @param pt0 Первая точка.
 * @param pt1 Вторая точка.
 * @param pt2 Третья точка.
 * @param precision Абсолютная точность вычислений.
 * @return Ориентация точек.
 *
 * Функция вычисляет ориентацию пары векторов (pt1, pt0) и (pt2, pt0).
 * В случае, если вектора положительно ориентированы, функция возвращает 1,
 * если отрицательно ориентированы, то функция возвращает -1 и функция
 * возвращает 0, если вектора коллинеарны.
 */
template<typename ValueType>
int Orientation(const Point<ValueType>& pt0,
                const Point<ValueType>& pt1,
                const Point<ValueType>& pt2,
                ValueType precision) {
  Point a = pt1 - pt0;
  Point b = pt2 - pt0;
  ValueType sa = a.X() * b.Y() - b.X() * a.Y();

  if (sa > precision)
    return 1;
  else if (sa < -precision)
    return -1;

  return 0;
}

}  // namespace geometry

#endif  // INCLUDE_ORIENTATION_HPP_
