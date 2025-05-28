/**
 * @file tests/polygon_test.cpp
 * @author Mikhail Lozhnikov
 *
 * Тесты для класса Point.
 */

#include <limits>
#include <utility>
#include <list>
#include <point.hpp>
#include <edge.hpp>
#include <polygon.hpp>
#include "test_core.hpp"
#include "test.hpp"


using geometry::Point;
using geometry::Edge;
using geometry::Rotation;

template<typename T>
static void BuildTest();

/**
 * @brief Набор тестов для класса geometry::Polygon.
 */
void TestPolygon() {
  TestSuite suite("TestPolygon");

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
  const T eps = std::numeric_limits<T>::epsilon() * T(1e3);
  {
    geometry::Polygon<T> polygon;

    REQUIRE_EQUAL(polygon.Size(), 0u);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices);

    REQUIRE_EQUAL(polygon.Size(), 2u);
    REQUIRE_CLOSE(polygon.Current()->X(), T(1.0), eps);
    REQUIRE_CLOSE(polygon.Current()->Y(), T(2.0), eps);
    REQUIRE_CLOSE(polygon.ClockWise()->X(), T(3.0), eps);
    REQUIRE_CLOSE(polygon.ClockWise()->Y(), T(4.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices, std::next(vertices.begin()));

    REQUIRE_EQUAL(polygon.Size(), 2u);
    REQUIRE_CLOSE(polygon.Current()->X(), T(3.0), eps);
    REQUIRE_CLOSE(polygon.Current()->Y(), T(4.0), eps);
    REQUIRE_CLOSE(polygon.ClockWise()->X(), T(1.0), eps);
    REQUIRE_CLOSE(polygon.ClockWise()->Y(), T(2.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices, std::next(vertices.begin()));
    const geometry::Polygon<T> other(polygon);

    REQUIRE_EQUAL(other.Size(), 2u);
    REQUIRE_CLOSE(other.Current()->X(), T(3.0), eps);
    REQUIRE_CLOSE(other.Current()->Y(), T(4.0), eps);
    REQUIRE_CLOSE(other.ClockWise()->X(), T(1.0), eps);
    REQUIRE_CLOSE(other.ClockWise()->Y(), T(2.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices, std::next(vertices.begin()));
    geometry::Polygon<T> other;

    other = polygon;

    REQUIRE_EQUAL(other.Size(), 2u);
    REQUIRE_CLOSE(other.Current()->X(), T(3.0), eps);
    REQUIRE_CLOSE(other.Current()->Y(), T(4.0), eps);
    REQUIRE_CLOSE(other.CounterClockWise()->X(), T(1.0), eps);
    REQUIRE_CLOSE(other.CounterClockWise()->Y(), T(2.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices, std::next(vertices.begin()));
    const geometry::Polygon<T> other(std::move(polygon));

    REQUIRE_EQUAL(other.Size(), 2u);
    REQUIRE_CLOSE(other.Current()->X(), T(3.0), eps);
    REQUIRE_CLOSE(other.Current()->Y(), T(4.0), eps);
    REQUIRE_CLOSE(other.CounterClockWise()->X(), T(1.0), eps);
    REQUIRE_CLOSE(other.CounterClockWise()->Y(), T(2.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices, std::next(vertices.begin()));
    geometry::Polygon<T> other;

    other = std::move(polygon);

    REQUIRE_EQUAL(other.Size(), 2u);
    REQUIRE_CLOSE(other.Current()->X(), T(3.0), eps);
    REQUIRE_CLOSE(other.Current()->Y(), T(4.0), eps);
    REQUIRE_CLOSE(other.ClockWise()->X(), T(1.0), eps);
    REQUIRE_CLOSE(other.ClockWise()->Y(), T(2.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices);

    REQUIRE_CLOSE(polygon.Vertices().front().X(), T(1.0), eps);
    REQUIRE_CLOSE(polygon.Vertices().front().Y(), T(2.0), eps);
    REQUIRE_CLOSE(polygon.Vertices().back().X(), T(3.0), eps);
    REQUIRE_CLOSE(polygon.Vertices().back().Y(), T(4.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    const geometry::Polygon<T> polygon(vertices);

    REQUIRE_CLOSE(polygon.Vertices().front().X(), T(1.0), eps);
    REQUIRE_CLOSE(polygon.Vertices().front().Y(), T(2.0), eps);
    REQUIRE_CLOSE(polygon.Vertices().back().X(), T(3.0), eps);
    REQUIRE_CLOSE(polygon.Vertices().back().Y(), T(4.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    const geometry::Polygon<T> polygon(vertices);
    Edge<T> edge = polygon.GetEdge();

    REQUIRE_CLOSE(edge.Origin().X(), T(1.0), eps);
    REQUIRE_CLOSE(edge.Origin().Y(), T(2.0), eps);
    REQUIRE_CLOSE(edge.Destination().X(), T(3.0), eps);
    REQUIRE_CLOSE(edge.Destination().Y(), T(4.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices, std::next(vertices.begin()));

    REQUIRE_EQUAL(polygon.Size(), 2u);
    REQUIRE_CLOSE(polygon.Current()->X(), T(3.0), eps);
    REQUIRE_CLOSE(polygon.Current()->Y(), T(4.0), eps);
    REQUIRE_CLOSE(polygon.Neighbor(Rotation::ClockWise)->X(), T(1.0), eps);
    REQUIRE_CLOSE(polygon.Neighbor(Rotation::ClockWise)->Y(), T(2.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    const geometry::Polygon<T> polygon(vertices, std::next(vertices.begin()));

    REQUIRE_EQUAL(polygon.Size(), 2u);
    REQUIRE_CLOSE(polygon.Current()->X(), T(3.0), eps);
    REQUIRE_CLOSE(polygon.Current()->Y(), T(4.0), eps);
    REQUIRE_CLOSE(polygon.Neighbor(Rotation::ClockWise)->X(), T(1.0), eps);
    REQUIRE_CLOSE(polygon.Neighbor(Rotation::ClockWise)->Y(), T(2.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices);

    REQUIRE_EQUAL(polygon.Size(), 2u);
    REQUIRE_CLOSE(polygon.Current()->X(), T(1.0), eps);
    REQUIRE_CLOSE(polygon.Current()->Y(), T(2.0), eps);

    typename std::list<Point<T>>::iterator it =
        polygon.Advance(Rotation::ClockWise);

    REQUIRE_CLOSE(it->X(), T(3.0), eps);
    REQUIRE_CLOSE(it->Y(), T(4.0), eps);
    REQUIRE_CLOSE(polygon.Current()->X(), T(3.0), eps);
    REQUIRE_CLOSE(polygon.Current()->Y(), T(4.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices);

    polygon.Insert(Point(T(5.0), T(6.0)));

    REQUIRE_EQUAL(polygon.Size(), 3u);
    REQUIRE_CLOSE(polygon.Current()->X(), T(5.0), eps);
    REQUIRE_CLOSE(polygon.Current()->Y(), T(6.0), eps);
    REQUIRE_CLOSE(polygon.ClockWise()->X(), T(3.0), eps);
    REQUIRE_CLOSE(polygon.ClockWise()->Y(), T(4.0), eps);
    REQUIRE_CLOSE(polygon.CounterClockWise()->X(), T(1.0), eps);
    REQUIRE_CLOSE(polygon.CounterClockWise()->Y(), T(2.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices);

    polygon.Insert(Point(T(5.0), T(6.0)));
    polygon.Remove(polygon.Current());

    REQUIRE_EQUAL(polygon.Size(), 2u);
    REQUIRE_CLOSE(polygon.Current()->X(), T(1.0), eps);
    REQUIRE_CLOSE(polygon.Current()->Y(), T(2.0), eps);
    REQUIRE_CLOSE(polygon.ClockWise()->X(), T(3.0), eps);
    REQUIRE_CLOSE(polygon.ClockWise()->Y(), T(4.0), eps);
  }

  {
    std::list<Point<T>> vertices = {
      Point(T(1.0), T(2.0)),
      Point(T(3.0), T(4.0))
    };
    geometry::Polygon<T> polygon(vertices);

    polygon.Insert(Point(T(5.0), T(6.0)));
    polygon.Insert(Point(T(7.0), T(8.0)));
    polygon.Insert(Point(T(9.0), T(10.0)));

    geometry::Polygon<T> other = polygon.Split(std::prev(polygon.Current(), 3));

    REQUIRE_EQUAL(polygon.Size(), 4u);
    REQUIRE_CLOSE(polygon.CounterClockWise()->X(), T(7.0), eps);
    REQUIRE_CLOSE(polygon.CounterClockWise()->Y(), T(8.0), eps);
    REQUIRE_CLOSE(polygon.Current()->X(), T(9.0), eps);
    REQUIRE_CLOSE(polygon.Current()->Y(), T(10.0), eps);
    REQUIRE_CLOSE(polygon.ClockWise()->X(), T(1.0), eps);
    REQUIRE_CLOSE(polygon.ClockWise()->Y(), T(2.0), eps);
    polygon.Advance(Rotation::ClockWise);
    REQUIRE_CLOSE(polygon.ClockWise()->X(), T(5.0), eps);
    REQUIRE_CLOSE(polygon.ClockWise()->Y(), T(6.0), eps);

    REQUIRE_EQUAL(other.Size(), 3u);
    REQUIRE_CLOSE(other.Current()->X(), T(1.0), eps);
    REQUIRE_CLOSE(other.Current()->Y(), T(2.0), eps);
    REQUIRE_CLOSE(other.ClockWise()->X(), T(9.0), eps);
    REQUIRE_CLOSE(other.ClockWise()->Y(), T(10.0), eps);
    REQUIRE_CLOSE(other.CounterClockWise()->X(), T(3.0), eps);
    REQUIRE_CLOSE(other.CounterClockWise()->Y(), T(4.0), eps);
  }
}
