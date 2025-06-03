/**
 * @file tests/alg_graham_test.cpp 
 * @author Almaz Sadykov
 * 
 * @brief Realization of a set of tests for the Graham scan algorithm
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

void TestAlgGraham(httplib::Client* cli) {
  TestSuite suite("TestAlgGraham");

  RUN_TEST_REMOTE(suite, cli, SimpleConvexTest);
  RUN_TEST_REMOTE(suite, cli, CollinearPointsTest);
  RUN_TEST_REMOTE(suite, cli, RandomPointsTest);
}

/**
 * @brief The simplest test with a convex polygon
 * 
 * @param cli pointer to HTTP client
 */

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

  httplib::Result res = cli->Post("/AlgGraham",
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

/**
 * @brief test with collinear points
 * 
 * @param cli pointer to HTTP client
 */

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

  httplib::Result res = cli->Post("/AlgGraham",
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

/**
 * @brief random test with the properties of a convex hull checking
 * 
 * @param cli pointer to HTTP client
 */

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

    httplib::Result res = cli->Post("/AlgGraham",
                                    input.dump(), "application/json");

    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(size, output["original_size"]);
    REQUIRE(output["hull_size"] <= size);

    // Convexity check
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
