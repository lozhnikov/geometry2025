/**
 * @file tests/point_test.cpp
 * @author Mikhail Lozhnikov
 *
 * Тесты для класса Point.
 */

#include <limits>
#include <point.hpp>
#include <edge.hpp>
#include "test_core.hpp"
#include "test.hpp"


using geometry::Point;
using geometry::Edge;
using geometry::Position;

template<typename T>
static void BuildTest();

/**
 * @brief Набор тестов для класса geometry::Point.
 */
void TestPoint() {
  TestSuite suite("TestPoint");

  RUN_TEST(suite, BuildTest<float>);
  RUN_TEST(suite, BuildTest<double>);
  RUN_TEST(suite, BuildTest<long double>);
}

/**
 * @brief Тест на компиляцию шаблона.
 *
 * @tparam T Тип данных, используемый для арифметики.
 */
template<typename T>
static void BuildTest() {
  const T eps = std::numeric_limits<T>::epsilon() * 1e3;

  {
    Point<T> point;

    REQUIRE_CLOSE(point.X(), T(0), eps);
    REQUIRE_CLOSE(point.Y(), T(0), eps);
    REQUIRE_CLOSE(point[0], T(0), eps);
    REQUIRE_CLOSE(point[1], T(0), eps);
  }

  {
    Point<T> point(T(1.0), T(2.0));

    REQUIRE_CLOSE(point.X(), T(1.0), eps);
    REQUIRE_CLOSE(point.Y(), T(2.0), eps);
    REQUIRE_CLOSE(point[0], T(1.0), eps);
    REQUIRE_CLOSE(point[1], T(2.0), eps);
  }

  {
    Point<T> point(T(1.0), T(2.0));
    const Point<T>& other = point;

    REQUIRE_CLOSE(other.X(), T(1.0), eps);
    REQUIRE_CLOSE(other.Y(), T(2.0), eps);
    REQUIRE_CLOSE(other[0], T(1.0), eps);
    REQUIRE_CLOSE(other[1], T(2.0), eps);
  }

  {
    Point<T> p1(T(1.0), T(2.0));
    Point<T> p2(T(3.0), T(4.0));
    Point<T> result = p1 + p2;

    REQUIRE_CLOSE(result.X(), T(4.0), eps);
    REQUIRE_CLOSE(result.Y(), T(6.0), eps);
  }

  {
    Point<T> p1(T(1.0), T(2.0));
    Point<T> p2(T(3.0), T(5.0));
    Point<T> result = p1 - p2;

    REQUIRE_CLOSE(result.X(), T(-2.0), eps);
    REQUIRE_CLOSE(result.Y(), T(-3.0), eps);
  }

  {
    Point<T> p1(T(1.0), T(2.0));
    Point<T> p2(T(3.0), T(5.0));

    REQUIRE_CLOSE(p1 * p2, T(13.0), eps);
  }

  {
    Point<T> p1(T(1.0), T(2.0));
    Point<T> p2(T(1.0), T(2.0));

    REQUIRE(Point<T>::IsEqual(p1, p2, eps));
  }

  {
    Point<T> p1(T(1.0), T(2.0));
    Point<T> p2(T(2.0), T(2.0));

    REQUIRE(p1 < p2);
    REQUIRE(p2 > p1);
  }

  {
    Point<T> p0(T(3.0), T(3.0));
    Point<T> p1(T(1.0), T(1.0));
    Point<T> p2(T(5.0), T(5.0));
    Edge<T> edge(p1, p2);

    REQUIRE(p0.Classify(p1, p2, eps) == Position::Between);
    REQUIRE(p0.Classify(edge, eps) == Position::Between);
  }

  {
    Point<T> p(T(1.0), T(1.0));

    REQUIRE_CLOSE(p.PolarAngle(eps), T(45.0), eps);
    REQUIRE_CLOSE(p.Length(), T(std::sqrt(2.0)), eps);
  }

  {
    Point<T> p(T(2.0), T(3.0));
    Point<T> p1(T(1.0), T(1.0));
    Point<T> p2(T(3.0), T(1.0));

    REQUIRE_CLOSE(p.Distance(Edge(p1, p2), eps), T(-2.0), eps);
  }

  {
    Point<T> p(T(2.0), T(3.0));
    T value = 4;
    Point<T> result = value * p;

    REQUIRE_CLOSE(result.X(), T(8.0), eps);
    REQUIRE_CLOSE(result.Y(), T(12.0), eps);
  }

  {
    Point<T> p(T(2.0), T(3.0));
    T value = 4;
    Point<T> result = p * value;

    REQUIRE_CLOSE(result.X(), T(8.0), eps);
    REQUIRE_CLOSE(result.Y(), T(12.0), eps);
  }
}
