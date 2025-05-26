/**
 * @file tests/convex_intersection_test.cpp 
 * @author German Semenov
 * 
 * @brief Realization of a set of tests for the convex intersection algorithm
 */

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <random>
#include <utility>
#include <algorithm>
#include <cmath>
#include "test_core.hpp"
#include "test.hpp"

static void StaticConvexIntersectionTest(httplib::Client* cli);
static void RandomConvexIntersectionTest(httplib::Client* cli);
static bool IsInsideConvexPolygon(const std::vector<std::pair<double, double>>& poly,
                                  const std::pair<double, double>& p);
static std::vector<std::pair<double, double>> GenerateConvexHull(std::vector<std::pair<double, double>> points);

void TestConvexIntersection(httplib::Client* cli) {
  TestSuite suite("TestConvexIntersection");

  RUN_TEST_REMOTE(suite, cli, StaticConvexIntersectionTest);
  RUN_TEST_REMOTE(suite, cli, RandomConvexIntersectionTest);
}

/**
 * @brief Проверка: точка внутри выпуклого многоугольника
 */
static bool IsInsideConvexPolygon(const std::vector<std::pair<double, double>>& poly,
                                  const std::pair<double, double>& p) {
  size_t n = poly.size();
  for (size_t i = 0; i < n; ++i) {
    auto [x1, y1] = poly[i];
    auto [x2, y2] = poly[(i + 1) % n];
    double cross = (x2 - x1) * (p.second - y1) - (y2 - y1) * (p.first - x1);
    if (cross < -1e-9) return false;
  }
  return true;
}

/**
 * @brief Алгоритм Грэхема: построение выпуклой оболочки
 */
static std::vector<std::pair<double, double>> GenerateConvexHull(std::vector<std::pair<double, double>> points) {
  if (points.size() <= 2) return points;

  auto orientation = [](const auto& a, const auto& b, const auto& c) {
    return (b.first - a.first) * (c.second - a.second)
         - (b.second - a.second) * (c.first - a.first);
  };

  std::swap(points[0], *std::min_element(points.begin(), points.end(),
    [](const auto& a, const auto& b) {
      return (a.second < b.second) || (a.second == b.second && a.first < b.first);
    }));

  auto base = points[0];
  std::sort(points.begin() + 1, points.end(), [&](const auto& a, const auto& b) {
    double ang = orientation(base, a, b);
    if (std::abs(ang) < 1e-9)
      return std::hypot(a.first - base.first, a.second - base.second)
           < std::hypot(b.first - base.first, b.second - base.second);
    return ang > 0;
  });

  std::vector<std::pair<double, double>> hull;
  for (const auto& pt : points) {
    while (hull.size() >= 2 &&
           orientation(hull[hull.size()-2], hull[hull.size()-1], pt) <= 0)
      hull.pop_back();
    hull.push_back(pt);
  }
  return hull;
}

/**
 * @brief Статический тест на пересечение квадратов
 */
static void StaticConvexIntersectionTest(httplib::Client* cli) {
  nlohmann::json input = {
    {"subject", {
      {{"x", 1.0}, {"y", 1.0}},
      {{"x", 4.0}, {"y", 1.0}},
      {{"x", 4.0}, {"y", 4.0}},
      {{"x", 1.0}, {"y", 4.0}}
    }},
    {"clip", {
      {{"x", 2.0}, {"y", 2.0}},
      {{"x", 5.0}, {"y", 2.0}},
      {{"x", 5.0}, {"y", 5.0}},
      {{"x", 2.0}, {"y", 5.0}}
    }}
  };

  auto res = cli->Post("/ConvexIntersection", input.dump(), "application/json");
  nlohmann::json output = nlohmann::json::parse(res->body);

  REQUIRE(output.contains("intersection"));
  REQUIRE(output["result_size"] > 0);

  std::vector<std::pair<double, double>> expected = {
    {2.0, 2.0}, {4.0, 2.0}, {4.0, 4.0}, {2.0, 4.0}
  };

  for (const auto& point : output["intersection"]) {
    auto p = std::make_pair(point["x"].get<double>(), point["y"].get<double>());
    REQUIRE(std::find(expected.begin(), expected.end(), p) != expected.end());
  }
}

/**
 * @brief Рандомный тест с выпуклыми многоугольниками и проверкой корректности результата
 */
static void RandomConvexIntersectionTest(httplib::Client* cli) {
  const int numTries = 30;
  const int maxPoints = 30;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> pointDist(5, maxPoints);
  std::uniform_real_distribution<double> coordDist(-100.0, 100.0);

  for (int it = 0; it < numTries; ++it) {
    int n1 = pointDist(gen);
    int n2 = pointDist(gen);

    std::vector<std::pair<double, double>> raw1, raw2;
    for (int i = 0; i < n1; ++i)
      raw1.emplace_back(coordDist(gen), coordDist(gen));
    for (int i = 0; i < n2; ++i)
      raw2.emplace_back(coordDist(gen), coordDist(gen));

    auto hull1 = GenerateConvexHull(raw1);
    auto hull2 = GenerateConvexHull(raw2);

    nlohmann::json input;
    for (size_t i = 0; i < hull1.size(); ++i) {
      input["subject"][i]["x"] = hull1[i].first;
      input["subject"][i]["y"] = hull1[i].second;
    }
    for (size_t i = 0; i < hull2.size(); ++i) {
      input["clip"][i]["x"] = hull2[i].first;
      input["clip"][i]["y"] = hull2[i].second;
    }

    auto res = cli->Post("/ConvexIntersection", input.dump(), "application/json");
    REQUIRE(res != nullptr);
    REQUIRE(res->status == 200);

    auto output = nlohmann::json::parse(res->body);

    REQUIRE(output.contains("intersection"));
    REQUIRE(output.contains("result_size"));

    for (const auto& point : output["intersection"]) {
      auto p = std::make_pair(point["x"].get<double>(), point["y"].get<double>());
      REQUIRE(IsInsideConvexPolygon(hull1, p));
      REQUIRE(IsInsideConvexPolygon(hull2, p));
    }
  }
}