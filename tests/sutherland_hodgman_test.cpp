/**
 * @file tests/sutherland_hodgman_test.cpp 
 * @brief Realization of a set of tests for the Sutherland-Hodgman algorithm
 */

#include <httplib.h>
#include <vector>
#include <random>
#include <utility>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"

static void SimpleClippingTest(httplib::Client* cli);
static void OutsideCaseTest(httplib::Client* cli);
static void RandomPolygonsTest(httplib::Client* cli);

void TestSutherlandHodgman(httplib::Client* cli) {
  TestSuite suite("TestSutherlandHodgman");

  RUN_TEST_REMOTE(suite, cli, SimpleClippingTest);
  RUN_TEST_REMOTE(suite, cli, OutsideCaseTest);
  RUN_TEST_REMOTE(suite, cli, RandomPolygonsTest);
}

/**
 * @brief Simple test with square clipping
 * 
 * @param cli pointer to HTTP client
 */
static void SimpleClippingTest(httplib::Client* cli) {
  nlohmann::json input = R"(
    {
      "subject_polygon": [
        {"x": 0.0, "y": 0.0},
        {"x": 2.0, "y": 0.0},
        {"x": 2.0, "y": 2.0},
        {"x": 0.0, "y": 2.0}
      ],
      "clip_polygon": [
        {"x": 1.0, "y": 1.0},
        {"x": 3.0, "y": 1.0},
        {"x": 3.0, "y": 3.0},
        {"x": 1.0, "y": 3.0}
      ]
    }
  )"_json;

  httplib::Result res = cli->Post("/SutherlandHodgman",
                                 input.dump(), "application/json");

  nlohmann::json output = nlohmann::json::parse(res->body);

  REQUIRE_EQUAL(4, output["result_size"]);
  REQUIRE_EQUAL(4, output["subject_size"]);
  REQUIRE_EQUAL(4, output["clip_size"]);

  std::vector<std::pair<double, double>> expected = {
    {1.0, 1.0}, {2.0, 1.0}, {2.0, 2.0}, {1.0, 2.0}};

  for (const auto& point : output["result"]) {
    auto p = std::make_pair(point["x"].get<double>(),
                           point["y"].get<double>());
    REQUIRE(std::find(expected.begin(), expected.end(), p) != expected.end());
  }
}

/**
 * @brief Test with completely outside case
 * 
 * @param cli pointer to HTTP client
 */
static void OutsideCaseTest(httplib::Client* cli) {
  nlohmann::json input = R"(
    {
      "subject_polygon": [
        {"x": 0.0, "y": 0.0},
        {"x": 1.0, "y": 0.0},
        {"x": 1.0, "y": 1.0},
        {"x": 0.0, "y": 1.0}
      ],
      "clip_polygon": [
        {"x": 2.0, "y": 2.0},
        {"x": 3.0, "y": 2.0},
        {"x": 3.0, "y": 3.0},
        {"x": 2.0, "y": 3.0}
      ]
    }
  )"_json;

  httplib::Result res = cli->Post("/SutherlandHodgman",
                                 input.dump(), "application/json");

  nlohmann::json output = nlohmann::json::parse(res->body);

  REQUIRE_EQUAL(0, output["result_size"]);
  REQUIRE_EQUAL(4, output["subject_size"]);
  REQUIRE_EQUAL(4, output["clip_size"]);
}

/**
 * @brief Random test with polygon clipping
 * 
 * @param cli pointer to HTTP client
 */
static void RandomPolygonsTest(httplib::Client* cli) {
  const int numTries = 50;
  const int maxPoints = 20;
  const double minCoord = 0.0;
  const double maxCoord = 10.0;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> sizeDist(3, maxPoints);
  std::uniform_real_distribution<double> coordDist(minCoord, maxCoord);

  // Fixed clip polygon (rectangle)
  nlohmann::json clipPolygon = nlohmann::json::array();
  clipPolygon.push_back({{"x", 2.0}, {"y", 2.0}});
  clipPolygon.push_back({{"x", 8.0}, {"y", 2.0}});
  clipPolygon.push_back({{"x", 8.0}, {"y", 8.0}});
  clipPolygon.push_back({{"x", 2.0}, {"y", 8.0}});

  for (int it = 0; it < numTries; it++) {
    nlohmann::json input;
    input["clip_polygon"] = clipPolygon;

    // Generate random subject polygon
    size_t size = sizeDist(gen);
    for (size_t i = 0; i < size; i++) {
      input["subject_polygon"][i]["x"] = coordDist(gen);
      input["subject_polygon"][i]["y"] = coordDist(gen);
    }

    httplib::Result res = cli->Post("/SutherlandHodgman",
                                   input.dump(), "application/json");

    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(size, output["subject_size"]);
    REQUIRE_EQUAL(4, output["clip_size"]);

    // Check if all resulting points are inside clip polygon
    for (const auto& point : output["result"]) {
      double x = point["x"].get<double>();
      double y = point["y"].get<double>();
      REQUIRE(x >= 2.0 && x <= 8.0);
      REQUIRE(y >= 2.0 && y <= 8.0);
    }
  }
}