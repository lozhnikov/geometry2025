/**
 * @file tests/my_polig_test.cpp
 * @author Nika Adzhindzhal
 *
 * Реализация набора тестов для алгоритма построения звездного полигона.
 */

#include <my_polig.hpp>
#include <point.hpp>
#include <polygon.hpp>
#include <vector>
#include <random>
#include <algorithm>
#include "test_core.hpp"
#include "test.hpp"
#include <httplib.h>
#include <nlohmann/json.hpp>

static void SimpleTest(httplib::Client* cli);
static void CollinearTest(httplib::Client* cli);
static void RandomTest(httplib::Client* cli);

void TestMyPolig(httplib::Client* cli) {
    TestSuite suite("TestMyPolig");

    RUN_TEST_REMOTE(suite, cli, SimpleTest);
    RUN_TEST_REMOTE(suite, cli, CollinearTest);
    RUN_TEST_REMOTE(suite, cli, RandomTest);
}

/**
 * @brief Простейший тест с упорядоченными точками.
 *
 * @param cli Указатель на HTTP клиент.
 */
static void SimpleTest(httplib::Client* cli) {
    nlohmann::json input = R"(
    {
      "points": [
        {"x": 0.0, "y": 0.0},
        {"x": 1.0, "y": 0.0},
        {"x": 0.0, "y": 1.0},
        {"x": -1.0, "y": 0.0},
        {"x": 0.0, "y": -1.0}
      ],
      "precision": 1e-9
    }
  )"_json;

    httplib::Result res = cli->Post("/StarPolygon", input.dump(), "application/json");

    // Проверка успешности запроса
    if (!res) {
        REQUIRE(false); // Запрос не удался
        return;
    }

    // Проверка статуса ответа
    if (res->status != 200) {
        REQUIRE_EQUAL(200, res->status); // Ожидался статус 200 OK
        return;
    }

    try {
        nlohmann::json output = nlohmann::json::parse(res->body);
        REQUIRE_EQUAL(5, output["hull_size"]);
        REQUIRE_EQUAL(5, output["original_size"]);

        std::vector<std::pair<double, double>> expected = {
          {0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {-1.0, 0.0}, {0.0, -1.0}
        };

        size_t idx = 0;
        for (const auto& point : output["polygon"]) {
            REQUIRE_EQUAL(expected[idx].first, point["x"].get<double>());
            REQUIRE_EQUAL(expected[idx].second, point["y"].get<double>());
            idx++;
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        REQUIRE(false); // Ошибка парсинга JSON
    }
}

/**
 * @brief Тест с коллинеарными точками.
 *
 * @param cli Указатель на HTTP клиент.
 */
static void CollinearTest(httplib::Client* cli) {
    nlohmann::json input = R"(
    {
      "points": [
        {"x": 0.0, "y": 0.0},
        {"x": 3.0, "y": 0.0},
        {"x": 1.0, "y": 0.0},
        {"x": 2.0, "y": 0.0}
      ],
      "precision": 1e-9
    }
  )"_json;

    httplib::Result res = cli->Post("/StarPolygon", input.dump(), "application/json");

    // Проверка успешности запроса
    if (!res) {
        REQUIRE(false);
        return;
    }

    // Проверка статуса ответа
    if (res->status != 200) {
        REQUIRE_EQUAL(200, res->status);
        return;
    }

    try {
        nlohmann::json output = nlohmann::json::parse(res->body);
        REQUIRE_EQUAL(4, output["hull_size"]);
        REQUIRE_EQUAL(4, output["original_size"]);

        std::vector<std::pair<double, double>> expected = {
          {0.0, 0.0}, {1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}
        };

        size_t idx = 0;
        for (const auto& point : output["polygon"]) {
            REQUIRE_EQUAL(expected[idx].first, point["x"].get<double>());
            REQUIRE_EQUAL(expected[idx].second, point["y"].get<double>());
            idx++;
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        REQUIRE(false);
    }
}

/**
 * @brief Тест со случайными точками.
 *
 * @param cli Указатель на HTTP клиент.
 */
static void RandomTest(httplib::Client* cli) {
    const int numTries = 50;
    const int maxPoints = 100;
    const double eps = 1e-9;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> sizeDist(3, maxPoints);
    std::uniform_real_distribution<double> coordDist(-100.0, 100.0);

    for (int it = 0; it < numTries; it++) {
        size_t size = sizeDist(gen);
        nlohmann::json input;

        input["points"][0]["x"] = 0.0;
        input["points"][0]["y"] = 0.0;

        for (size_t i = 1; i < size; i++) {
            input["points"][i]["x"] = coordDist(gen);
            input["points"][i]["y"] = coordDist(gen);
        }

        input["precision"] = 1e-9;

        httplib::Result res = cli->Post("/StarPolygon", input.dump(), "application/json");

        // Проверка успешности запроса
        if (!res) {
            REQUIRE(false);
            continue;
        }

        // Проверка статуса ответа
        if (res->status != 200) {
            REQUIRE_EQUAL(200, res->status);
            continue;
        }

        try {
            nlohmann::json output = nlohmann::json::parse(res->body);
            REQUIRE_EQUAL(size, output["hull_size"]);
            REQUIRE_EQUAL(size, output["original_size"]);

            std::vector<geometry::Point<double>> polygon;
            for (const auto& pt : output["polygon"]) {
                polygon.emplace_back(pt["x"].get<double>(), pt["y"].get<double>());
            }

            geometry::Point<double> origin = polygon[0];
            for (size_t i = 1; i < polygon.size(); ++i) {
                geometry::Point<double> current = polygon[i] - origin;
                geometry::Point<double> prev_point = polygon[i - 1] - origin;

                double angle_prev = prev_point.PolarAngle(eps);
                double angle_current = current.PolarAngle(eps);

                if (angle_prev > angle_current + eps) {
                    REQUIRE(false); // Угол должен быть неубывающим
                }
                else if (std::abs(angle_prev - angle_current) < eps) {
                    double len_prev = prev_point.Length();
                    double len_current = current.Length();
                    if (len_prev > len_current + eps) {
                        REQUIRE(false); // При равных углах расстояние должно быть неубывающим
                    }
                }
            }
        }
        catch (const nlohmann::json::parse_error& e) {
            REQUIRE(false);
        }
    }
}
