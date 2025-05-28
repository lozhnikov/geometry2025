/**
 * @file include/polygon.hpp
 * @author Mikhail Lozhnikov
 *
 * Определение класса многоугольника.
 */

#ifndef INCLUDE_POLYGON_HPP_
#define INCLUDE_POLYGON_HPP_

#include <list>
#include <common.hpp>

namespace geometry {

template<typename T>
class Point;

template<typename T>
class Edge;

/**
 * @brief Класс многоугольника.
 *
 * @tparam T Тип данных, используемый для арифметики.
 * @tparam Container Тип контейнера для хранения вершин.
 *
 * Предполагается, что многоугольник хранит вершины в порядке обхода по часовой
 * стрелке.
 */
template<typename T, typename Container = std::list<Point<T>>>
class Polygon {
 private:
  //! Вершины многоугольника.
  Container vertices;
  //! Текущая вершина.
  typename Container::iterator current;

 public:
  //! Тип данных, используемый для арифметики.
  using ValueType = T;
  //! Тип контейнера для хранения вершин.
  using ContainerType = Container;

  /**
   * @brief Конструктор по-умолчанию.
   *
   * Создать пустой многоугольник.
   */
  Polygon();

  /**
   * @brief Создать многоугольник из заданного набора вершин.
   *
   * @param vertices Вершины.
   *
   * Функция создаёт многоугольник из заданного набора вершин.
   * Первая вершина в контейнере становится текущей. Предполагается,
   * что вершины лежат в контейнере в порядке обхода по часовой стрелке.
   */
  explicit Polygon(const Container& vertices);

  /**
   * @brief Создать многоугольник из заданного набора вершин.
   *
   * @param vertices Вершины.
   * @param position Текущая вершина.
   *
   * Функция создаёт многоугольник из заданного набора вершин. Предполагается,
   * что вершины лежат в контейнере в порядке обхода по часовой стрелке.
   */
  Polygon(const Container& vertices,
          typename Container::iterator position);

  /**
   * @brief Конструктор копирования.
   *
   * @param other Копируемый многоугольник.
   */
  Polygon(const Polygon& other);

  /**
   * @brief Конструктор перемещения.
   *
   * @param other Перемещаемый многоугольник.
   */
  Polygon(Polygon&& other);

  /**
   * @brief Операция копирующего присваивания.
   *
   * @param other Копируемый многоугольник.
   * @return Функция возвращает ссылку на текущий объект.
   */
  Polygon& operator=(const Polygon& other);

  /**
   * @brief Операция перемещающего присваивания.
   *
   * @param other Перемещаемый многоугольник.
   * @return Возвращает ссылку на текущий объект.
   */
  Polygon& operator=(Polygon&& other);

  /**
   * @brief Получить список вершин.
   * @return Функция возвращает контейнер, содержащий вершины.
   */
  const Container& Vertices() const { return vertices; }
  /**
   * @brief Изменить список вершин.
   * @return Функция возвращает ссылку на контейнер, содержащий вершины.
   */
  Container& Vertices() { return vertices; }

  /**
   * @brief Модифицировать итератор на текущую вершину.
   * @return Функция возвращает ссылку на итератор на текущую вершину.
   */
  typename Container::iterator& Current() { return current; }
  /**
   * @brief Получить константный итератор на текущую вершину.
   * @return Функция возвращает константный итератор на текущую вершину.
   */
  typename Container::const_iterator Current() const { return current; }

  /**
   * @brief Получить количество вершин.
   * @return Функция возвращает количество вершин в многоугольнике.
   */
  std::size_t Size() const { return vertices.size(); }

  /**
   * @brief Получить текущее ребро.
   * @return Функция возвращает ребро, соединяющее текущую точку и следующую
   * точку.
   */
  Edge<T> GetEdge() const;

  /**
   * @brief Получить итератор на соседнюю вершину по часовой стрелке.
   * @return Функция возвращает итератор на следующую вершину.
   */
  typename Container::iterator ClockWise();

  /**
   * @brief Получить итератор на соседнюю вершину против часовой стрелки.
   * @return Функция возвращает итератор на предыдущую вершину.
   */
  typename Container::iterator CounterClockWise();

  /**
   * @brief Получить константный итератор на соседнюю вершину по часовой
   * стрелке.
   * @return Функция возвращает константный итератор на следующую вершину.
   */
  typename Container::const_iterator ClockWise() const;

  /**
   * @brief Получить константный итератор на соседнюю вершину против часовой
   * стрелки.
   * @return Функция возвращает константный итератор на предыдущую вершину.
   */
  typename Container::const_iterator CounterClockWise() const;

  /**
   * @brief Получить итератор на соседнюю вершину.
   *
   * @param rotation Направление обхода.
   * @return Функция возвращает итератор на соседнюю вершину.
   *
   * Функция возвращает итератор на соседнюю вершину в заданном
   * направлении обхода.
   */
  typename Container::iterator Neighbor(Rotation rotation);

  /**
   * @brief Получить итератор на соседнюю вершину.
   *
   * @param rotation Направление обхода.
   * @return Функция возвращает константный итератор на соседнюю вершину.
   *
   * Функция возвращает константный итератор на соседнюю вершину в заданном
   * направлении обхода.
   */
  typename Container::const_iterator Neighbor(Rotation rotation) const;

  /**
   * @brief Переместиться на соседнюю вершину.
   *
   * @param rotation Направление обхода.
   * @return Функция возвращает итератор на соседнюю вершину.
   *
   * Функция перемещает текущую позицию на соседнюю вершину в заданном
   * направлении обхода и возвращает итератор на эту вершину.
   */
  typename Container::iterator Advance(Rotation rotation);

  /**
   * @brief Вставить вершину в многоугольник.
   *
   * @param point Точка, которую нужно вставить.
   * @return Функция возвращает итератор на добавленную вершину.
   *
   * Функция добавляет вершину в многоугольник после текущей и
   * перемещает текущую позицию на добавленную вершину. Функция возвращает
   * итератор на добавленную вершину.
   */
  typename Container::iterator Insert(const Point<T>& point);

  /**
   * @brief Удалить вершину из многоугольника.
   *
   * @param position Итератор на вершину, которую нужно удалить.
   *
   * Функция удаляет указанную вершину и возвращает итератор на предыдущую
   * вершину. Если многоугольник становится пустым, то возвращается
   * итератор на конец контейнера.
   */
  void Remove(typename Container::iterator position);

  /**
   * @brief Разбить многоугольник на два многоугольника.
   *
   * @param position Вторая вершина, в которой нужно произвести разрез.
   * @return Функция возвращает вторую часть многоугольника.
   *
   * Функция разбивает многоугольник на две части. Для этого многоугольник
   * разрезается в двух местах: в текущей позиции и в точке position.
   * Затем концы получившихся ломаных соединяются рёбрами. В исходном
   * многоугольнике текущая точка не меняется, а следующей вершиной по часовой
   * стрелке становится вершина position. Во втором многоугольнике
   * текущей вершиной становится вершина, соответствующая итератору
   * position, а следующей вершиной становится вершина, соответствующая
   * текущей вершине исходного многоугольника.
   */
  Polygon Split(typename Container::iterator position);
};

}  // namespace geometry

#include <polygon_impl.hpp>

#endif  // INCLUDE_POLYGON_HPP_
