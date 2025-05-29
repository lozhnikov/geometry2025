/**
 * @file tests/check_convex_test.cpp
 * @author Daria Slepenkova
 *
 * @brief Realization of a set of tests for checking the convexity of a polygon vertex.
 */

#include <httplib.h>
#include <vector>
#include <random>
#include <utility>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"
#include "polygon.hpp"
#include "point.hpp"

static void FirstTest(httplib::Client* cli);
static void SecondTest(httplib::Client* cli);
static void ThirdTest(httplib::Client* cli);
//static void RandomTest(httplib::Client* cli);

void TestCheckConvex(httplib::Client* cli) {
  TestSuite suite("TestCheckConvex");

  RUN_TEST_REMOTE(suite, cli, FirstTest);
  RUN_TEST_REMOTE(suite, cli, SecondTest);
  RUN_TEST_REMOTE(suite, cli, ThirdTest);
//  RUN_TEST_REMOTE(suite, cli, RandomTest);
}

/**
 * @brief the first test with a convex polygon
 *
 * @param cli pointer to HTTP client
 */

static void FirstTest(httplib::Client* cli) {
  nlohmann::json input = R"(
    {
      "points" : [
        {"x": 0.0, "y": 0.0},
        {"x": 0.0, "y": 1.0},
        {"x": 1.0, "y": 1.0},
        {"x": 1.0, "y": 0.0}
      ]
    }
  )"_json;

  httplib::Result res = cli->Post("/CheckConvex",
                                  input.dump(), "application/json");

  nlohmann::json output = nlohmann::json::parse(res->body);

  REQUIRE_EQUAL(4, output["number_of_points"]);

  std::vector<std::pair<double, double>> expected = {
    {0.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}};

  for (const auto& point : output["convex_points"]) {
    auto p = std::make_pair(point["x"].get<double>(),
                            point["y"].get<double>());
    REQUIRE(std::find(expected.begin(), expected.end(), p) != expected.end());
  }
}

/**
 * @brief the second test is for a polygon with concave and convex vertices
 *
 * @param cli pointer to HTTP client
 */

static void SecondTest(httplib::Client* cli) {
  nlohmann::json input = R"(
    {
      "points" : [
        {"x": 0.0, "y": 0.0},
        {"x": 2.0, "y": 2.0},
        {"x": 1.0, "y": 3.0},
        {"x": 3.0, "y": 5.0},
        {"x": 4.0, "y": 4.0},
        {"x": 5.0, "y": 4.0},
        {"x": 6.0, "y": 1.0}
      ]
    }
  )"_json;

  httplib::Result res = cli->Post("/CheckConvex",
                                  input.dump(), "application/json");

  nlohmann::json output = nlohmann::json::parse(res->body);

  REQUIRE_EQUAL(5, output["number_of_points"]);

  std::vector<std::pair<double, double>> expected = {
    {0.0, 0.0}, {1.0, 3.0}, {3.0, 5.0}, {5.0, 4.0}, {6.0, 1.0}};

    for (const auto& point : output["convex_points"]) {
      auto p = std::make_pair(point["x"].get<double>(),
                              point["y"].get<double>());
      REQUIRE(std::find(expected.begin(), expected.end(), p)
              != expected.end());
    }
}

/**
 * @brief the third test is for a polygon with concave and convex vertices
 *
 * @param cli pointer to HTTP client
 */


static void ThirdTest(httplib::Client* cli) {
  nlohmann::json input = R"(
    {
      "points" : [
        {"x": 1.0, "y": 1.0},
        {"x": 2.0, "y": 3.0},
        {"x": 3.0, "y": 2.0},
        {"x": 4.0, "y": 4.0},
        {"x": 4.0, "y": 1.0}
      ]
    }
  )"_json;

  httplib::Result res = cli->Post("/CheckConvex",
                                  input.dump(), "application/json");

  nlohmann::json output = nlohmann::json::parse(res->body);

  REQUIRE_EQUAL(4, output["number_of_points"]);

  std::vector<std::pair<double, double>> expected = {
    {1.0, 1.0}, {2.0, 3.0}, {4.0, 4.0}, {4.0, 1.0}};

  for (const auto& point : output["convex_points"]) {
    auto p = std::make_pair(point["x"].get<double>(),
                            point["y"].get<double>());
    REQUIRE(std::find(expected.begin(), expected.end(), p) != expected.end());
  }
}




