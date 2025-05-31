/**
 * @file closest_pair_test.cpp
 * @brief Набор тестов для алгоритма поиска ближайшей пары точек
 */

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <random>
#include <cmath>
#include "test_core.hpp"
#include "test.hpp"

static void ClosestPairSimpleTest(httplib::Client* cli);
static void ClosestPairRandomTest(httplib::Client* cli);
static void ClosestPairColinearTest(httplib::Client* cli);
static void ClosestPairTinyDistanceTest(httplib::Client* cli);

// Главный вызов
void TestClosestPair(httplib::Client* cli) {
  TestSuite suite("TestClosestPair");

  RUN_TEST_REMOTE(suite, cli, ClosestPairSimpleTest);
  RUN_TEST_REMOTE(suite, cli, ClosestPairRandomTest);
  RUN_TEST_REMOTE(suite, cli, ClosestPairColinearTest);
  RUN_TEST_REMOTE(suite, cli, ClosestPairTinyDistanceTest);
}

/**
 * @brief Простой тест — ожидаем расстояние между (0,0) и (1,1)
 */
static void ClosestPairSimpleTest(httplib::Client* cli) {
  nlohmann::json input = R"(
    {
      "points": [
        {"x": 0.0, "y": 0.0},
        {"x": 3.0, "y": 4.0},
        {"x": 1.0, "y": 1.0}
      ]
    }
  )"_json;

  httplib::Result res = cli->Post("/ClosestPair", input.dump(), "application/json");

  REQUIRE(res != nullptr);
  REQUIRE_EQUAL(res->status, 200);

  nlohmann::json output = nlohmann::json::parse(res->body);
  double expected = std::sqrt(2.0);
  REQUIRE(std::abs(output["distance"].get<double>() - expected) < 1e-5);
}

/**
 * @brief Случай с генерацией случайных точек — просто проверка, что работает
 */
static void ClosestPairRandomTest(httplib::Client* cli) {
  const int numTests = 10;
  const int numPoints = 100;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dist(0.0, 100.0);

  for (int t = 0; t < numTests; t++) {
    nlohmann::json input;
    input["points"] = nlohmann::json::array();

    for (int i = 0; i < numPoints - 2; i++) {
      input["points"].push_back({{"x", dist(gen)}, {"y", dist(gen)}});
    }

    // Добавим две точки с очень маленьким расстоянием между ними
    double baseX = dist(gen);
    double baseY = dist(gen);
    input["points"].push_back({{"x", baseX}, {"y", baseY}});
    input["points"].push_back({{"x", baseX + 1e-10}, {"y", baseY}});

    httplib::Result res = cli->Post("/ClosestPair", input.dump(), "application/json");
    REQUIRE(res != nullptr);
    REQUIRE_EQUAL(res->status, 200);

    auto output = nlohmann::json::parse(res->body);
    double distance = output["distance"].get<double>();
    REQUIRE(std::abs(distance - 1e-10) < 1e-9);
  }
}

/**
 * @brief Все точки лежат на одной прямой — расстояние между соседними = 1
 */
static void ClosestPairColinearTest(httplib::Client* cli) {
  nlohmann::json input;
  for (int i = 0; i <= 10; i++) {
    input["points"].push_back({{"x", static_cast<double>(i)}, {"y", 0.0}});
  }

  httplib::Result res = cli->Post("/ClosestPair", input.dump(), "application/json");

  REQUIRE(res != nullptr);
  REQUIRE_EQUAL(res->status, 200);

  auto output = nlohmann::json::parse(res->body);
  REQUIRE(std::abs(output["distance"].get<double>() - 1.0) < 1e-6);
}

/**
 * @brief Тест на очень близкие точки
 */
static void ClosestPairTinyDistanceTest(httplib::Client* cli) {
  nlohmann::json input = {
    {"points", {
      {{"x", 0.0}, {"y", 0.0}},
      {{"x", 1e-9}, {"y", 0.0}},
      {{"x", 100.0}, {"y", 100.0}}
    }}
  };

  httplib::Result res = cli->Post("/ClosestPair", input.dump(), "application/json");

  REQUIRE(res != nullptr);
  REQUIRE_EQUAL(res->status, 200);

  auto output = nlohmann::json::parse(res->body);
  REQUIRE(output["distance"].get<double>() < 1e-8);
}
