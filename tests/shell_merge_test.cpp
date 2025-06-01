/**
 * @file tests/shell_merge_test.cpp
 * @author Щербинин Виталий
 * @brief Реализация набора тестов для алгоритма ShellMerge
 */

#include <httplib.h>
#include <vector>
#include <random>
#include <utility>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"

static void SimpleMergeTest(httplib::Client* cli);
static void CollinearMergeTest(httplib::Client* cli);
static void RandomMergeTest(httplib::Client* cli);

/**
 * @brief Простой тест слияния двуз выпуклых оболочек
 *
 * @param cli указатель на http клиент для отправки запросов на сервер
 */
void TestShellMerge(httplib::Client* cli) {
  TestSuite suite("TestShellMerge");

  RUN_TEST_REMOTE(suite, cli, SimpleMergeTest);
  RUN_TEST_REMOTE(suite, cli, CollinearMergeTest);
  RUN_TEST_REMOTE(suite, cli, RandomMergeTest);
}

/**
 * @brief Простой тест слияния двух выпуклых оболочек
 */
static void SimpleMergeTest(httplib::Client* cli) {
  nlohmann::json input = {
    {"left", {{0.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}}},
    {"right", {{2.0, 0.0}, {2.0, 1.0}, {1.0, 0.0}}}
  };

  httplib::Result res = cli->Post
  ("/ShellMerge", input.dump(), "application/json");

  nlohmann::json output = nlohmann::json::parse(res->body);
  REQUIRE(output.is_array());
  REQUIRE(output.size() >= 3);
}

/**
 * @brief Простой тест слияния двух выпуклых оболочек
 */
static void CollinearMergeTest(httplib::Client* cli) {
  nlohmann::json input = {
    {"left", {{0.0, 0.0}, {1.0, 1.0}, {2.0, 2.0}}},
    {"right", {{3.0, 3.0}, {4.0, 4.0}, {2.0, 0.0}}}
  };

  httplib::Result res = cli->Post
  ("/ShellMerge", input.dump(), "application/json");

  nlohmann::json output = nlohmann::json::parse(res->body);
  REQUIRE(output.is_array());
  REQUIRE(output.size() >= 3);
}

/**
 * @brief Случайный тест слияния двух выпуклых оболочек
 */
static void RandomMergeTest(httplib::Client* cli) {
  const int maxPoints = 100;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> sizeDist(5, maxPoints);
  std::uniform_real_distribution<double> coordDist(-100.0, 100.0);

  nlohmann::json input;

  size_t sizeL = sizeDist(gen);
  size_t sizeR = sizeDist(gen);
  for (size_t i = 0; i < sizeL; i++) {
    input["left"].push_back({coordDist(gen), coordDist(gen)});
  }
  for (size_t i = 0; i < sizeR; i++) {
    input["right"].push_back({coordDist(gen), coordDist(gen)});
  }

  httplib::Result res = cli->Post
  ("/ShellMerge", input.dump(), "application/json");

  nlohmann::json output = nlohmann::json::parse(res->body);
  REQUIRE(output.is_array());
  REQUIRE(output.size() >= 3);
}
