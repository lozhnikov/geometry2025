/**
 * @file include/point.hpp
 * @author Mikhail Lozhnikov
 *
 * Определение и реализация некоторых методов класса точки.
 */

#ifndef INCLUDE_POINT_HPP_
#define INCLUDE_POINT_HPP_

#include <cmath>
#include <utility>
#include <algorithm>
#include <common.hpp>

namespace geometry {

template<typename T>
class Edge;

/**
 * @brief Класс точки.
 *
 * @tparam T Тип данных, используемый для арифметики.
 */
template<typename T>
class Point {
 private:
  T data[2];

 public:
  //! Тип данных, используемый для арифметики.
  using ValueType = T;

  /**
   * @brief Создать объект класса точки.
   *
   * @param x \f$ X \f$ координата точки.
   * @param y \f$ Y \f$ координита точки.
   */
  explicit Point(T x = 0, T y = 0) noexcept {
    data[0] = x;
    data[1] = y;
  }

  /**
   * @brief Получить значение \f$ X \f$ координаты.
   * @return \f$ X \f$ координата.
   */
  T X() const { return data[0]; }

  /**
   * @brief Изменить значение \f$ X \f$ координаты.
   * @return \f$ X \f$ координата.
   */
  T& X() { return data[0]; }

  /**
   * @brief Получить значение \f$ Y \f$ координаты.
   * @return \f$ Y \f$ координата.
   */
  T Y() const { return data[1]; }

  /**
   * @brief Изменить значение \f$ Y \f$ координаты.
   * @return \f$ Y \f$ координата.
   */
  T& Y() { return data[1]; }

  /**
   * @brief Сложить две точки.
   *
   * @param other Вторая точка.
   * @return Результат операции.
   *
   * Функция вычисляет сумму текущей точки и точки other и возвращает результат.
   */
  Point operator+(const Point& other) const {
    return Point(X() + other.X(), Y() + other.Y());
  }

  /**
   * @brief Вычислить разность двух точек.
   *
   * @param other Вторая точки.
   * @return Результат операции.
   *
   * Функция вычисляет разность текущей точки и точки other и возвращает
   * результат.
   */
  Point operator-(const Point& other) const {
    return Point(X() - other.X(), Y() - other.Y());
  }

  /**
   * @brief Вычислить скалярное произведение двух точек.
   *
   * @param other Вторая точка.
   * @return Скалярное произведение двух точек.
   *
   * Евклидово скалярное произведение по формуле
   * \f[
   *   x^2 + y^2.
   * \f]
   */
  T operator*(const Point& other) const {
    return X() * other.X() + Y() * other.Y();
  }

  /**
   * @brief Получить координату точки.
   *
   * @param index Номер координаты.
   * @return Соответствующая координата точки.
   *
   * Функция возвращает \f$ \mathrm{index} \f$ координату точки.
   */
  T operator[] (int index) const {
    return data[index];
  }

  /**
   * @brief Изменить координату точки.
   *
   * @param index Номер координаты.
   * @return Соответствующая координата точки.
   *
   * Функция возвращает ссылку на \f$ \mathrm{index} \f$ координату точки.
   */
  T& operator[] (int index) {
    return data[index];
  }

  /**
   * @brief Вычислить, совпадают ли две точки.
   *
   * @param left Первая точка.
   * @param right Вторая точки.
   * @param precision Относительная точность вычислений.
   * @return Булево значение, отражающее, совпадают ли две точки.
   *
   * Функция позволяет узнать, совпадают ли две точки.
   */
  static bool IsEqual(const Point& left,
                      const Point& right,
                      T precision) {
    if (std::fabs(left.X() - right.X()) <=
        precision * std::max(std::fabs(left.X()), std::fabs(right.X())) &&
        std::fabs(left.Y() - right.Y()) <=
        precision * std::max(std::fabs(left.Y()), std::fabs(right.Y())))
      return true;

    return false;
  }

  /**
   * @brief Сравнить две точки покоординатно в лексикографическом порядке.
   *
   * @param other Вторая точка.
   * @return Булево значение, отражающее результат сравнения.
   *
   * Функция возвращает true, если текущая точка лексикографически
   * меньше второй и false в противном случае.
   */
  bool operator<(const Point& other) const {
    return std::pair(X(), Y()) < std::pair(other.X(), other.Y());
  }

  /**
   * @brief Сравнить две точки покоординатно в лексикографическом порядке.
   *
   * @param other Вторая точка.
   * @return Булево значение, отражающее результат сравнения.
   *
   * Функция возвращает true, если текущая точка лексикографически
   * больше второй и false в противном случае.
   */
  bool operator>(const Point& other) const {
    return std::pair(X(), Y()) > std::pair(other.X(), other.Y());
  }

  /**
   * @brief Определить положение точки относительно прямой.
   *
   * @param p0 Начало отрезка, задающего прямую.
   * @param p1 Конец отрезка, задающего прямую.
   * @param precision Абсолютная точность вычислений.
   * @return Результат классификации.
   *
   * Функция определяет положение текущей точки относительно прямой (p0, p1).
   */
  Position Classify(const Point& p0, const Point& p1, T precision) const {
    Point a = p1 - p0;
    Point b = *this - p0;
    T sa = a.X() * b.Y() - b.X() * a.Y();

    if (sa > precision)
      return Position::Left;
    else if (sa < -precision)
      return Position::Right;
    else if ((a.X() * b.X() < 0) || (a.Y() * b.Y() < 0))
      return Position::Behind;
    else if (a.Length() < b.Length())
      return Position::Beyond;
    else if (IsEqual(p0, *this, precision))
      return Position::Origin;
    else if (IsEqual(p1, *this, precision))
      return Position::Destination;

    return Position::Between;
  }

  /**
   * @brief Определить положение точки относительно прямой.
   *
   * @param edge Отрезок, задающий прямую.
   * @param precision Абсолютная точность вычислений.
   * @return Результат классификации.
   *
   * Функция определяет положение текущей точки относительно прямой,
   * заданной отрезком edge.
   */
  Position Classify(const Edge<T>& edge, T precision) const;

  /**
   * @brief Вычислить полярный угол.
   *
   * @param precision Абсолютная точность вычислений.
   * @return Полярный угол в градусах.
   *
   * Функция вычисляет полярный угол точки в градусах.
   */
  T PolarAngle(T precision) const {
    if (std::fabs(data[0]) < precision && std::fabs(data[1]) < precision)
      return -1;

    if (std::fabs(data[0]) < precision)
      return Y() > 0 ? 90 : 270;

    T theta = T(std::atan(Y() / X()));
    theta *= T(360.0 / (2 * PI));

    if (X() > 0)
      return Y() > 0 ? theta : 360 + theta;
    else
      return 180 + theta;
  }

  /**
   * @brief Вычислить расстояние от начала координат до текущей точки.
   * @return Расстояние от начала координат до текущей точки.
   */
  T Length() const {
    return std::sqrt(X() * X() + Y() * Y());
  }

  /**
   * @brief Вычислить расстояние от точки до прямой.
   *
   * @param edge Ребро, задающее прямую.
   * @param precision Абсолютная точность вычислений.
   * @return Расстояние от текущей точки до прямой.
   *
   * Функция вычисляет расстояние от текущей точки до прямой,
   * проходящей через ребро edge.
   */
  T Distance(const Edge<T>& edge, T precision) const;
};

/**
 * @brief Операция умножения точки на константу.
 *
 * @tparam ValueType Тип данных, используемый для арифметики.
 *
 * @param value Константа.
 * @param point Точка.
 * @return Результат операции.
 */
template<typename ValueType>
Point<ValueType> operator*(ValueType value, const Point<ValueType>& point) {
  return Point<ValueType>(value * point.X(), value * point.Y());
}

/**
 * @brief Операция умножения точки на константу.
 *
 * @tparam ValueType Тип данных, используемый для арифметики.
 *
 * @param point Точка.
 * @param value Константа.
 * @return Результат операции.
 */
template<typename ValueType>
Point<ValueType> operator*(const Point<ValueType>& point, ValueType value) {
  return Point<ValueType>(value * point.X(), value * point.Y());
}

}  // namespace geometry

#include <point_impl.hpp>

#endif  // INCLUDE_POINT_HPP_
