/**
 * @file tests/convex_polygon_intersection_test.cpp
 * @brief Тесты для алгоритма пересечения выпуклых многоугольников
 */

#include <geometry/convex_polygon_intersection.hpp>
#include <tests/test.hpp>
#include <random>
#include <vector>
#include <iostream>

namespace geometry {
namespace test {

/**
 * @brief Тестирование базовых случаев пересечения
 */
void TestSimpleCases() {
  // Пересекающиеся квадраты
  {
    std::vector<Point<double>> poly1 = {{0,0}, {2,0}, {2,2}, {0,2}};
    std::vector<Point<double>> poly2 = {{1,1}, {3,1}, {3,3}, {1,3}};
    auto result = convexPolygonIntersection(poly1, poly2);
    ASSERT(result.size() == 4);
  }

  // Один многоугольник полностью внутри другого
  {
    std::vector<Point<double>> poly1 = {{0,0}, {3,0}, {3,3}, {0,3}};
    std::vector<Point<double>> poly2 = {{1,1}, {2,1}, {2,2}, {1,2}};
    auto result = convexPolygonIntersection(poly1, poly2);
    ASSERT(result.size() == poly2.size());
  }

  // Многоугольники не пересекаются
  {
    std::vector<Point<double>> poly1 = {{0,0}, {1,0}, {1,1}, {0,1}};
    std::vector<Point<double>> poly2 = {{2,2}, {3,2}, {3,3}, {2,3}};
    auto result = convexPolygonIntersection(poly1, poly2);
    ASSERT(result.empty());
  }
}

/**
 * @brief Тестирование на случайных данных
 */
void TestRandomCases() {
  std::mt19937 gen(42);
  std::uniform_real_distribution<double> dist(0.0, 10.0);

  for (int i = 0; i < 10; ++i) {
    // Генерация случайного выпуклого многоугольника
    auto generate_polygon = [&](int n) {
      std::vector<Point<double>> points(n);
      for (auto& p : points) {
        p = {dist(gen), dist(gen)};
      }
      // Сортировка по углу для выпуклости
      std::sort(points.begin(), points.end(), [¢er](const auto& a, const auto& b) {
        return atan2(a.y-center.y, a.x-center.x) < atan2(b.y-center.y, b.x-center.x);
      });
      return points;
    };

    auto poly1 = generate_polygon(5 + i%5);
    auto poly2 = generate_polygon(5 + i%5);

    try {
      auto result = convexPolygonIntersection(poly1, poly2);
      std::cout << "Test " << i << ": intersection has " 
                << result.size() << " points\n";
    } catch (const std::exception& e) {
      std::cerr << "Test " << i << " failed: " << e.what() << "\n";
      FAIL();
    }
  }
}

} // namespace test
} // namespace geometry

/**
 * @brief Основная тестовая функция
 */
void TestConvexPolygonIntersection() {
  TEST("Simple intersection cases", 
       geometry::test::TestSimpleCases());
  TEST("Random polygons intersection", 
       geometry::test::TestRandomCases());
}