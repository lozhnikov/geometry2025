/**
 * @file tests/my_polig_test.cpp
 * @author Nika Adzhindzhal
 *
 * Реализация набора тестов для алгоритма построения звездного полигона.
 */

#include <my_polig.hpp>
#include <httplib.h>
#include <vector>
#include <random>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"

namespace geometry {

    // Функция для сравнения точек с учетом точности
    template<typename T>
    bool PointEquals(const Point<T>& p1, const Point<T>& p2, T epsilon) {
        return std::abs(p1.x - p2.x) < epsilon && std::abs(p1.y - p2.y) < epsilon;
    }

} // namespace geometry

static void EmptyTest(httplib::Client* cli);
static void SinglePointTest(httplib::Client* cli);
static void ThreePointsTest(httplib::Client* cli);
static void CollinearPointsTest(httplib::Client* cli);
static void RandomTest(httplib::Client* cli);

void TestMyPolig(httplib::Client* cli) {
    TestSuite suite("TestMyPolig");

    RUN_TEST_REMOTE(suite, cli, EmptyTest);
    RUN_TEST_REMOTE(suite, cli, SinglePointTest);
    RUN_TEST_REMOTE(suite, cli, ThreePointsTest);
    RUN_TEST_REMOTE(suite, cli, CollinearPointsTest);
    RUN_TEST_REMOTE(suite, cli, RandomTest);
}

static void EmptyTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "points": [],
        "precision": 0.0001
    })"_json;

    httplib::Result res = cli->Post("/TestMyPolig", input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(output.contains("polygon"), false);
}

static void SinglePointTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "points": [[0, 0]],
        "precision": 0.0001
    })"_json;

    httplib::Result res = cli->Post("/TestMyPolig", input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(output["polygon"].size(), 1);
    REQUIRE_EQUAL(output["polygon"][0][0], 0);
    REQUIRE_EQUAL(output["polygon"][0][1], 0);
}

static void ThreePointsTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "points": [[0, 0], [1, 0], [0, 1]],
        "precision": 0.0001
    })"_json;

    httplib::Result res = cli->Post("/TestMyPolig", input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(output["polygon"].size(), 3);

    // Проверяем наличие всех точек
    std::vector<std::vector<double>> expected = { {0,0}, {1,0}, {0,1} };
    for (const auto& pt : expected) {
        bool found = false;
        for (const auto& poly_pt : output["polygon"]) {
            if (std::abs(poly_pt[0] - pt[0]) < 1e-9 &&
                std::abs(poly_pt[1] - pt[1]) < 1e-9) {
                found = true;
                break;
            }
        }
        REQUIRE_EQUAL(found, true);
    }
}

static void CollinearPointsTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "points": [[0, 0], [1, 0], [2, 0], [3, 0]],
        "precision": 0.0001
    })"_json;

    httplib::Result res = cli->Post("/TestMyPolig", input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(output["polygon"].size(), 4);

    // Ожидаемый порядок: сначала самые удаленные точки
    std::vector<std::vector<double>> expected_order = { {0,0}, {3,0}, {2,0}, {1,0} };
    for (size_t i = 0; i < expected_order.size(); ++i) {
        REQUIRE_EQUAL(output["polygon"][i][0], expected_order[i][0]);
        REQUIRE_EQUAL(output["polygon"][i][1], expected_order[i][1]);
    }
}

static void RandomTest(httplib::Client* cli) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-10.0, 10.0);
    const double precision = 1e-6;

    for (int i = 0; i < 10; ++i) {
        nlohmann::json input;
        input["precision"] = precision;

        // Генерируем от 5 до 20 точек
        std::uniform_int_distribution<size_t> size_dist(5, 20);
        size_t num_points = size_dist(gen);
        std::vector<std::vector<double>> points;

        // Первая точка - начало координат
        points.push_back({ 0.0, 0.0 });

        // Остальные точки случайные (исключаем дубликаты)
        for (size_t j = 1; j < num_points; ++j) {
            double x = dist(gen);
            double y = dist(gen);
            points.push_back({ x, y });
        }

        input["points"] = points;

        httplib::Result res = cli->Post("/TestMyPolig", input.dump(), "application/json");
        nlohmann::json output = nlohmann::json::parse(res->body);

        // Проверка количества точек
        REQUIRE_EQUAL(output["polygon"].size(), num_points);

        // Проверка наличия всех точек
        for (const auto& pt : points) {
            bool found = false;
            for (const auto& poly_pt : output["polygon"]) {
                if (std::abs(poly_pt[0] - pt[0]) < precision &&
                    std::abs(poly_pt[1] - pt[1]) < precision) {
                    found = true;
                    break;
                }
            }
            REQUIRE_EQUAL(found, true);
        }
    }
}
