/**
 * @file tests/orientation_test.cpp
 * @author Mikhail Lozhnikov
 *
 * Тесты для функции geometry::Orientation().
 */

#include <limits>
#include <point.hpp>
#include <orientation.hpp>
#include "test_core.hpp"
#include "test.hpp"


using geometry::Point;
using geometry::Orientation;

template<typename T>
static void BuildTest();

/**
 * @brief Набор тестов для функции geometry::Orientation().
 */
void TestOrientation() {
  TestSuite suite("TestOrientation");

  RUN_TEST(suite, BuildTest<float>);
  RUN_TEST(suite, BuildTest<double>);
  RUN_TEST(suite, BuildTest<long double>);
}

/**
 * @brief Тесты на компиляцию для функции geometry::Orientation().
 *
 * @tparam T Тип данных, используемый для арифметики.
 */
template<typename T>
static void BuildTest() {
  const T eps = std::numeric_limits<T>::epsilon() * 1e3;
  {
    Point<T> p0(T(0.0), T(0.0));
    Point<T> p1(T(1.0), T(0.0));
    Point<T> p2(T(0.0), T(1.0));

    REQUIRE_EQUAL(Orientation(p0, p1, p2, eps), 1);
  }
}
