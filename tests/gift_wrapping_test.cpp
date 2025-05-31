// tests/gift_wrapping_test.cpp
/**
 * @file tests/gift_wrapping_test.cpp
 * @author Ivan Kuznetsov
 *
 * @brief Realization of a set of tests for the Gift wrapping algorithm
 */

#include <httplib.h>
#include <vector>
#include <random>
#include <utility>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"

static void SimpleConvexTest(httplib::Client* cli);
static void CollinearPointsTest(httplib::Client* cli);
static void RandomPointsTest(httplib::Client* cli);
static void PointInConvexHullTest(httplib::Client* cli);

void TestGiftWrapping(httplib::Client* cli) {
  TestSuite suite("TestGiftWrapping");

  RUN_TEST_REMOTE(suite, cli, SimpleConvexTest);
  RUN_TEST_REMOTE(suite, cli, CollinearPointsTest);
  RUN_TEST_REMOTE(suite, cli, RandomPointsTest);
  RUN_TEST_REMOTE(suite, cli, PointInConvexHullTest);
}

static void SimpleConvexTest(httplib::Client* cli) {
  nlohmann::json input = R"(
    {
      "points" : [
        {"x": 0.0, "y": 0.0}, 
        {"x": 1.0, "y": 1.0},
        {"x": 2.0, "y": 0.0},
        {"x": 1.0, "y": -1.0}
      ]
    }
  )"_json;

  httplib::Result res = cli->Post("/GiftWrapping",
                                  input.dump(), "application/json");

  nlohmann::json output = nlohmann::json::parse(res->body);

  REQUIRE_EQUAL(4, output["hull_size"]);
  REQUIRE_EQUAL(4, output["original_size"]);

  std::vector<std::pair<double, double>> expected = {
    {0.0, 0.0}, {1.0, 1.0}, {2.0, 0.0}, {1.0, -1.0}};

  for (const auto& point : output["convex_hull"]) {
    auto p = std::make_pair(point["x"].get<double>(),
                            point["y"].get<double>());
    REQUIRE(std::find(expected.begin(), expected.end(), p) != expected.end());
  }
}

static void CollinearPointsTest(httplib::Client* cli) {
  nlohmann::json input = R"(
    {
      "points" : [
        {"x": 0.0, "y": 0.0},
        {"x": 1.0, "y": 1.0},
        {"x": 2.0, "y": 2.0},
        {"x": 3.0, "y": 1.0},
        {"x": 2.0, "y": 0.0},
        {"x": 1.0, "y": 0.0}
      ]
    }
  )"_json;

  httplib::Result res = cli->Post("/GiftWrapping",
                                  input.dump(), "application/json");

  nlohmann::json output = nlohmann::json::parse(res->body);

  REQUIRE_EQUAL(4, output["hull_size"]);
  REQUIRE_EQUAL(6, output["original_size"]);

  std::vector<std::pair<double, double>> expected = {
    {0.0, 0.0}, {2.0, 2.0}, {3.0, 1.0}, {2.0, 0.0}};

    for (const auto& point : output["convex_hull"]) {
      auto p = std::make_pair(point["x"].get<double>(),
                              point["y"].get<double>());
      REQUIRE(std::find(expected.begin(), expected.end(), p)
              != expected.end());
    }
}

static void RandomPointsTest(httplib::Client* cli) {
  const int numTries = 50;
  const int maxPoints = 100;
  const double eps = 1e-9;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> sizeDist(5, maxPoints);
  std::uniform_real_distribution<double> coordDist(-100.0, 100.0);

  for (int it = 0; it < numTries; it++) {
    size_t size = sizeDist(gen);
    nlohmann::json input;

    for (size_t i = 0; i < size; i++) {
      input["points"][i]["x"] = coordDist(gen);
      input["points"][i]["y"] = coordDist(gen);
    }

    httplib::Result res = cli->Post("/GiftWrapping",
                                    input.dump(), "application/json");

    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(size, output["original_size"]);
    REQUIRE(output["hull_size"] <= size);

    if (output["hull_size"] >= 3) {
      for (size_t i = 0; i < output["hull_size"]; i++) {
        size_t j = (i + 1) % output["hull_size"].get<size_t>();
        int k = (i + 2) % output["hull_size"].get<size_t>();

        auto& a = output["convex_hull"][i];
        auto& b = output["convex_hull"][j];
        auto& c = output["convex_hull"][k];

        double cross = (b["x"].get<double>() - a["x"].get<double>()) *
                       (c["y"].get<double>() - a["y"].get<double>()) -
                       (b["y"].get<double>() - a["y"].get<double>()) *
                       (c["x"].get<double>() - a["x"].get<double>());

        REQUIRE(cross > -eps);
      }
    }
  }
}

double CalculatePolygonArea(const std::vector<std::pair<double,
                            double>>& points) {
    double area = 0.0;
    size_t n = points.size();
    for (size_t i = 0; i < n; i++) {
        size_t j = (i + 1) % n;
        area += points[i].first * points[j].second;
        area -= points[j].first * points[i].second;
    }
    return std::abs(area) / 2.0;
}


bool IsPointInConvexPolygon(const std::vector<std::pair<double,
                            double>>& polygon,
                           const std::pair<double, double>& point,
                           double eps = 1e-9) {
    double total_area = 0.0;
    size_t n = polygon.size();

    for (size_t i = 0; i < n; i++) {
        size_t j = (i + 1) % n;
        
        double a = (polygon[j].second - point.second)*(polygon[j].second - point.second) + (polygon[j].first - point.first)*(polygon[j].first - point.first);
        double b = (polygon[i].second - point.second)*(polygon[i].second - point.second) + (polygon[i].first - point.first)*(polygon[i].first - point.first);
        double c = (polygon[j].second - polygon[i].second)*(polygon[j].second - polygon[i].second) + (polygon[j].first - polygon[i].first)*(polygon[j].first - polygon[i].first);
        double p = (a + b + c)/2;

        double area = sqrt((p)*(p - a)*(p - b)*(p - c));
        total_area += area;
    }

    double polygon_area = CalculatePolygonArea(polygon);
    return std::abs(total_area - polygon_area) < eps;
}
static void PointInConvexHullTest(httplib::Client* cli) {
    const int numTests = 50;
    const double eps = 1e-6;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> size_dist(5, 20);
    std::uniform_real_distribution<double> coord_dist(-10.0, 10.0);

    for (int test = 0; test < numTests; test++) {
        nlohmann::json input;
        size_t size = size_dist(gen);
        for (size_t i = 0; i < size; i++) {
            input["points"][i]["x"] = coord_dist(gen);
            input["points"][i]["y"] = coord_dist(gen);
        }


        httplib::Result res = cli->Post("/GiftWrapping",
                                      input.dump(),
                                      "application/json");
        nlohmann::json output = nlohmann::json::parse(res->body);

        std::vector<std::pair<double, double>> convex_hull;
        for (const auto& p : output["convex_hull"]) {
            convex_hull.emplace_back(p["x"].get<double>(),
                                   p["y"].get<double>());
        }


        for (const auto& point : input["points"]) {
            std::pair<double, double> p = {
                point["x"].get<double>(),
                point["y"].get<double>()
            };

            bool is_in_hull = IsPointInConvexPolygon(convex_hull,
                                                     p, eps);


            REQUIRE(is_in_hull);
        }


        if (convex_hull.size() >= 3) {
            double max_x = convex_hull[0].first;
            for (const auto& p : convex_hull) {
                if (p.first > max_x) max_x = p.first;
            }
            std::pair<double, double> outside_point = {max_x + 1.0, 0.0};
            bool outside_in_hull = IsPointInConvexPolygon(convex_hull,
                                                          outside_point, eps);
            REQUIRE(!outside_in_hull);
        }
    }
}
