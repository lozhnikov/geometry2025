/**
 * @file include/edge.hpp
 * @author Mikhail Lozhnikov
 *
 * Определение класса ребра.
 */

#ifndef INCLUDE_EDGE_HPP_
#define INCLUDE_EDGE_HPP_

#include <common.hpp>

namespace geometry {

template<typename T>
class Point;

/**
 * @brief Класс ребра.
 *
 * @tparam T Тип данных, используемый для арифметики.
 */
template<typename T>
class Edge {
 private:
  //! Начало отрезка.
  Point<T> origin;
  //! Конец отрезка.
  Point<T> destination;

 public:
  //! Тип данных, используемый для арифметики.
  using ValueType = T;

  /**
   * @brief Конструктор по-умолчанию для ребра.
   *
   * Создаёт ребро с координатами (0, 0) и (1, 0).
   */
  Edge();

  /**
   * @brief Специальный конструктор для ребра.
   *
   * @param origin Начало ребра.
   * @param destination Конец ребра.
   *
   * Создать ребро от одной заданной точки до другой заданной точки.
   */
  Edge(const Point<T>& origin, const Point<T>& destination);

  /**
   * @brief Получить точку начало ребра.
   * @return Начало ребра.
   */
  const Point<T>& Origin() const { return origin; }

  /**
   * @brief Изменить точку начало ребра.
   * @return Начало ребра.
   */
  Point<T>& Origin() { return origin; }

  /**
   * @brief Получить точку конца ребра.
   * @return Конец ребра.
   */
  const Point<T>&  Destination() const { return destination; }

  /**
   * @brief Получить точку конца ребра.
   * @return Конец ребра.
   */
  Point<T>&  Destination() { return destination; }

  /**
   * @brief Повернуть ребро по часовой стрелке на 90 градусов.
   * @return Текущий объект.
   *
   * Функция поворачивает ребро по часовой стрелке на 90 градусов и возвращает
   * ссылку на текущий объект.
   */
  Edge& Rotate();

  /**
   * @brief Развернуть ребро на 180 градусов.
   * @return Текущий объект.
   *
   * Функция разворачивает ребро на 180 градусов и возвращает ссылку
   * на текущий объект.
   */
  Edge& Flip();

  /**
   * @brief Получить точку на прямой, содержащей данное ребро.
   *
   * @param t Параметр, определяющий положение точки на прямой.
   * @return Точка на прямой.
   *
   * Функция возвращает точку по формуле
   * \f[
   *   \mathrm{origin} +  t \cdot \mathrm{destination}.
   * \f]
   */
  Point<T> Value(T t) const;

  /**
   * @brief Определить, пересекаются ли две прямые, содержащие рёбра.
   *
   * @param edge Второе ребро.
   * @param t В эту переменную запишется значение параметра, соответствующего
   * точке пересечения.
   * @param precision Абсолютная точность вычислений.
   * @return Тип пересечения.
   *
   * Функция определяет, пересекаются ли две прямые, содержащие рёбра,
   * и записывает значение параметра, соответствующего точке пересечения.
   */
  Intersection Intersect(const Edge& edge, T* t, T precision) const;

  /**
   * @brief Определить, пересекаются ли два ребра.
   *
   * @param edge Второе ребро.
   * @param t В эту переменную запишется значение параметра, соответствующего
   * точке пересечения.
   * @param precision Абсолютная точность вычислений.
   * @return Тип пересечения.
   *
   * Функция определяет, пересекаются ли два ребра, и записывает значение
   * параметра, соответствующего точке пересечения.
   */
  Intersection Cross(const Edge& edge, T* t, T precision);

  /**
   * @brief Функция проверяет, является ли ребро вертикальным.
   * @return Булево значение, которое показывает, является ли ребро
   * вертикальным.
   *
   * @param precision Относительная точность вычислений.
   */
  bool IsVertical(T precision) const;

  /**
   * @brief Функция вычисляет коэффициент угла наклона.
   *
   * @param precision Относительная точность вычислений.
   * @return Коэффициент угла наклона.
   *
   * Функция вычисляет коэффициент угла наклона. Если ребро вертикальное,
   * то возвращается максимальное значение соответствующего типа данных.
   */
  T Slope(T precision) const;

  /**
   * @brief Получить \f$ Y \f$  координату точки на прямой, содержащей ребро,
   * в зависимости от значения \f$ X \f$ координаты.
   *
   * @param x Значение \f$ x \f$ координаты.
   * @param precision Относительная точность вычислений.
   * @return Значение \f$ Y \f$ координаты.
   */
  T Y(T x, T precision) const;
};

}  // namespace geometry

#include <edge_impl.hpp>

#endif  // INCLUDE_EDGE_HPP_
