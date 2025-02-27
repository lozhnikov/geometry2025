/**
 * @file tests/edge_test.cpp
 * @author Mikhail Lozhnikov
 *
 * Тесты для класса Edge.
 */

#include <limits>
#include <edge.hpp>
#include "test_core.hpp"
#include "test.hpp"


using geometry::Point;
using geometry::Edge;
using geometry::Intersection;

template<typename T>
static void BuildTest();

/**
 * @brief Набор тестов для класса geometry::Edge.
 */
void TestEdge() {
  TestSuite suite("TestEdge");

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
  const T eps = std::numeric_limits<T>::epsilon();

  {
    Point<T> origin;
    Point<T> destination;
    Edge<T> edge;

    REQUIRE_CLOSE(edge.Origin().X(), T(0), eps);
    REQUIRE_CLOSE(edge.Origin().Y(), T(0), eps);
    REQUIRE_CLOSE(edge.Destination().X(), T(1), eps);
    REQUIRE_CLOSE(edge.Destination().Y(), T(0), eps);
  }

  {
    Point<T> origin(T(1.0), T(2.0));
    Point<T> destination(T(3.0), T(4.0));
    const Edge<T> edge(origin, destination);

    REQUIRE_CLOSE(edge.Origin().X(), T(1.0), eps);
    REQUIRE_CLOSE(edge.Origin().Y(), T(2.0), eps);
    REQUIRE_CLOSE(edge.Destination().X(), T(3.0), eps);
    REQUIRE_CLOSE(edge.Destination().Y(), T(4.0), eps);
  }

  {
    Point<T> origin(T(1.0), T(1.0));
    Point<T> destination(T(3.0), T(3.0));
    Edge<T> edge(origin, destination);
    edge.Rotate();

    REQUIRE_CLOSE(edge.Origin().X(), T(1.0), eps);
    REQUIRE_CLOSE(edge.Origin().Y(), T(3.0), eps);
    REQUIRE_CLOSE(edge.Destination().X(), T(3.0), eps);
    REQUIRE_CLOSE(edge.Destination().Y(), T(1.0), eps);
  }

  {
    Point<T> origin(T(1.0), T(2.0));
    Point<T> destination(T(3.0), T(4.0));
    Edge<T> edge(origin, destination);
    edge.Flip();

    REQUIRE_CLOSE(edge.Origin().X(), T(3.0), eps);
    REQUIRE_CLOSE(edge.Origin().Y(), T(4.0), eps);
    REQUIRE_CLOSE(edge.Destination().X(), T(1.0), eps);
    REQUIRE_CLOSE(edge.Destination().Y(), T(2.0), eps);
  }

  {
    Point<T> origin(T(1.0), T(1.0));
    Point<T> destination(T(3.0), T(2.0));
    Edge<T> edge(origin, destination);
    Point<T> result = edge.Value(0.5);

    REQUIRE_CLOSE(result.X(), T(2.0), eps);
    REQUIRE_CLOSE(result.Y(), T(1.5), eps);
  }

  {
    Point<T> p1(T(1.0), T(1.0));
    Point<T> p2(T(3.0), T(2.0));
    Point<T> p3(T(1.0), T(2.0));
    Point<T> p4(T(3.0), T(1.0));
    T t;
    Edge<T> e1(p1, p2);
    Edge<T> e2(p3, p4);

    REQUIRE(e1.Intersect(e2, &t, eps) == Intersection::Skew);
    REQUIRE(e1.Cross(e2, &t, eps) == Intersection::SkewCross);
  }

  {
    Point<T> p1(T(1.0), T(1.0));
    Point<T> p2(T(1.0), T(2.0));
    Edge<T> e(p1, p2);

    REQUIRE(e.IsVertical(eps));
  }

  {
    Point<T> p1(T(1.0), T(1.0));
    Point<T> p2(T(2.0), T(2.0));
    Edge<T> e(p1, p2);

    REQUIRE_CLOSE(e.Slope(eps), T(1.0), eps);
  }

  {
    Point<T> p1(T(1.0), T(1.0));
    Point<T> p2(T(2.0), T(2.0));
    Edge<T> e(p1, p2);
    T x = 1.5;

    REQUIRE_CLOSE(e.Y(x, eps), T(1.5), eps);
  }
}
