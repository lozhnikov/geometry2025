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

 // Оператор вывода для httplib::Error
namespace httplib {
    std::ostream& operator<<(std::ostream& os, const Error& err) {
        switch (err) {
        case Error::Success: return os << "Success";
        case Error::Unknown: return os << "Unknown";
        case Error::Connection: return os << "Connection";
        default: return os << "Error(" << static_cast<int>(err) << ")";
        }
    }
} // namespace httplib

namespace geometry {
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
    TestSuite suite("TestStarPolygon");

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

    httplib::Result res = cli->Post("/StarPolygon", input.dump(), "application/json");

    // Проверяем успешность запроса и код ответа
    REQUIRE_EQUAL(res.error(), httplib::Error::Success);
    REQUIRE(res != nullptr);
    REQUIRE_EQUAL(res->status, 200);

    try {
        nlohmann::json output = nlohmann::json::parse(res->body);
        // Для пустого набора точек ожидаем пустой полигон или отсутствие поля
        REQUIRE(output.contains("polygon"));
        REQUIRE(output["polygon"].is_array());
        REQUIRE_EQUAL(output["polygon"].size(), 0);
    }
    catch (...) {
        REQUIRE_EQUAL("JSON parse error", "No error");
    }
}

static void SinglePointTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "points": [[0, 0]],
        "precision": 0.0001
    })"_json;

    httplib::Result res = cli->Post("/StarPolygon", input.dump(), "application/json");

    REQUIRE_EQUAL(res.error(), httplib::Error::Success);
    REQUIRE(res != nullptr);
    REQUIRE_EQUAL(res->status, 200);

    try {
        nlohmann::json output = nlohmann::json::parse(res->body);
        REQUIRE(output.contains("polygon"));
        REQUIRE_EQUAL(output["polygon"].size(), 1);
        REQUIRE_EQUAL(output["polygon"][0][0].get<double>(), 0);
        REQUIRE_EQUAL(output["polygon"][0][1].get<double>(), 0);
    }
    catch (...) {
        REQUIRE_EQUAL("JSON parse error", "No error");
    }
}

static void ThreePointsTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "points": [[0, 0], [1, 0], [0, 1]],
        "precision": 0.0001
    })"_json;

    httplib::Result res = cli->Post("/StarPolygon", input.dump(), "application/json");

    REQUIRE_EQUAL(res.error(), httplib::Error::Success);
    REQUIRE(res != nullptr);
    REQUIRE_EQUAL(res->status, 200);

    try {
        nlohmann::json output = nlohmann::json::parse(res->body);
        REQUIRE(output.contains("polygon"));
        REQUIRE_EQUAL(output["polygon"].size(), 3);

        std::vector<std::vector<double>> expected = { {0,0}, {1,0}, {0,1} };
        for (const auto& pt : expected) {
            bool found = false;
            for (const auto& poly_pt : output["polygon"]) {
                double x = poly_pt[0].get<double>();
                double y = poly_pt[1].get<double>();
                if (std::abs(x - pt[0]) < 1e-9 &&
                    std::abs(y - pt[1]) < 1e-9) {
                    found = true;
                    break;
                }
            }
            REQUIRE_EQUAL(found, true);
        }
    }
    catch (...) {
        REQUIRE_EQUAL("JSON parse error", "No error");
    }
}

static void CollinearPointsTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "points": [[0, 0], [1, 0], [2, 0], [3, 0]],
        "precision": 0.0001
    })"_json;

    httplib::Result res = cli->Post("/StarPolygon", input.dump(), "application/json");

    REQUIRE_EQUAL(res.error(), httplib::Error::Success);
    REQUIRE(res != nullptr);
    REQUIRE_EQUAL(res->status, 200);

    try {
        nlohmann::json output = nlohmann::json::parse(res->body);
        REQUIRE(output.contains("polygon"));
        REQUIRE_EQUAL(output["polygon"].size(), 4);

        // Для коллинеарных точек порядок может быть разным
        std::vector<std::vector<double>> expected_points = { {0,0}, {1,0}, {2,0}, {3,0} };
        for (const auto& pt : expected_points) {
            bool found = false;
            for (const auto& poly_pt : output["polygon"]) {
                double x = poly_pt[0].get<double>();
                double y = poly_pt[1].get<double>();
                if (std::abs(x - pt[0]) < 1e-9 &&
                    std::abs(y - pt[1]) < 1e-9) {
                    found = true;
                    break;
                }
            }
            REQUIRE_EQUAL(found, true);
        }
    }
    catch (...) {
        REQUIRE_EQUAL("JSON parse error", "No error");
    }
}

static void RandomTest(httplib::Client* cli) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-10.0, 10.0);
    const double precision = 1e-6;

    for (int i = 0; i < 3; ++i) { // Уменьшил количество тестов для скорости
        nlohmann::json input;
        input["precision"] = precision;

        std::uniform_int_distribution<size_t> size_dist(5, 10);
        size_t num_points = size_dist(gen);
        std::vector<std::vector<double>> points;

        for (size_t j = 0; j < num_points; ++j) {
            points.push_back({ dist(gen), dist(gen) });
        }

        input["points"] = points;

        httplib::Result res = cli->Post("/StarPolygon", input.dump(), "application/json");

        REQUIRE_EQUAL(res.error(), httplib::Error::Success);
        REQUIRE(res != nullptr);
        REQUIRE_EQUAL(res->status, 200);

        try {
            nlohmann::json output = nlohmann::json::parse(res->body);
            REQUIRE(output.contains("polygon"));
            REQUIRE_EQUAL(output["polygon"].size(), num_points);

            for (const auto& pt : points) {
                bool found = false;
                for (const auto& poly_pt : output["polygon"]) {
                    double x = poly_pt[0].get<double>();
                    double y = poly_pt[1].get<double>();
                    if (std::abs(x - pt[0]) < precision &&
                        std::abs(y - pt[1]) < precision) {
                        found = true;
                        break;
                    }
                }
                REQUIRE_EQUAL(found, true);
            }
        }
        catch (...) {
            REQUIRE_EQUAL("JSON parse error", "No error");
        }
    }
}
