/**
 * @file tests/my_polig_test.cpp
 * @author Nika Adzhindzhal
 *
 * Реализация набора тестов для алгоритма построения звездного многоугольника.
 */
#include <my_polig.hpp>
#include <point.hpp>
#include <polygon.hpp>
#include <vector>
#include <random>
#include <algorithm>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"

namespace geometry {

    template<typename T>
    int PolarCmpForTest(const Point<T>& p, const Point<T>& q, const Point<T>& origin, T precision) {
        Point<T> vp = p - origin;
        Point<T> vq = q - origin;
        T angle_p = vp.PolarAngle(precision);
        T angle_q = vq.PolarAngle(precision);

        if (angle_p < angle_q) return -1;
        if (angle_p > angle_q) return 1;

        T len_p = vp.Length();
        T len_q = vq.Length();

        if (len_p < len_q) return -1;
        if (len_p > len_q) return 1;
        return 0;
    }

} // namespace geometry

static void SimpleTest(httplib::Client* cli);
static void RandomTest(httplib::Client* cli);

void TestMyPolig(httplib::Client* cli) {
    TestSuite suite("TestMyPolig");
    RUN_TEST_REMOTE(suite, cli, SimpleTest);
    RUN_TEST_REMOTE(suite, cli, RandomTest);
}

static void SimpleTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "id": 1,
        "precision": 1e-5,
        "points": [
            {"x": 0.5, "y": 0.5},
            {"x": 1.0, "y": 0.0},
            {"x": 0.0, "y": 1.0},
            {"x": -1.0, "y": 0.0},
            {"x": 0.0, "y": -1.0}
        ]
    })"_json;

    httplib::Result res = cli->Post("/StarPolygon", input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(1, output["id"]);
    REQUIRE_EQUAL(5, output["vertices"].size());

    // Ожидаемый порядок вершин
    std::vector<geometry::Point<double>> expected = {
        {0.5, 0.5}, {0.0, -1.0}, {-1.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}
    };

    for (size_t i = 0; i < 5; i++) {
        REQUIRE_EQUAL(expected[i].x, output["vertices"][i]["x"]);
        REQUIRE_EQUAL(expected[i].y, output["vertices"][i]["y"]);
    }
}

static void RandomTest(httplib::Client* cli) {
    const int numTries = 50;
    const double precision = 1e-5;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> coord(-10.0, 10.0);
    std::uniform_int_distribution<size_t> sizeDist(5, 20);

    for (int it = 0; it < numTries; it++) {
        size_t size = sizeDist(gen);
        nlohmann::json input;
        input["id"] = it;
        input["precision"] = precision;
        input["points"] = nlohmann::json::array();

        // Генерация origin (не слишком близко к нулю)
        geometry::Point<double> origin;
        do {
            origin.x = coord(gen);
            origin.y = coord(gen);
        } while (std::abs(origin.x) < 0.1 && std::abs(origin.y) < 0.1);

        input["points"].push_back({ {"x", origin.x}, {"y", origin.y} });

        // Генерация остальных точек
        std::vector<geometry::Point<double>> points;
        points.push_back(origin);

        for (size_t i = 1; i < size; i++) {
            geometry::Point<double> p;
            do {
                p.x = coord(gen);
                p.y = coord(gen);
            } while (std::abs(p.x - origin.x) < 0.1 &&
                std::abs(p.y - origin.y) < 0.1);

            input["points"].push_back({ {"x", p.x}, {"y", p.y} });
            points.push_back(p);
        }

        httplib::Result res = cli->Post("/StarPolygon", input.dump(), "application/json");
        nlohmann::json output = nlohmann::json::parse(res->body);

        REQUIRE_EQUAL(it, output["id"]);
        REQUIRE_EQUAL(size, output["vertices"].size());

        // Проверка порядка вершин
        std::vector<geometry::Point<double>> vertices;
        for (const auto& vertex : output["vertices"]) {
            vertices.emplace_back(vertex["x"], vertex["y"]);
        }

        REQUIRE_EQUAL(origin, vertices[0]);

        std::vector<geometry::Point<double>> polyPoints(vertices.begin() + 1, vertices.end());
        std::vector<geometry::Point<double>> inputPoints(points.begin() + 1, points.end());

        // Сортировка для проверки
        std::sort(inputPoints.begin(), inputPoints.end(),
            [&](const geometry::Point<double>& a, const geometry::Point<double>& b) {
                int cmp = geometry::PolarCmpForTest(a, b, origin, precision);
                if (cmp != 0) return cmp > 0;
                return (a - origin).Length() < (b - origin).Length();
            });

        for (size_t i = 0; i < polyPoints.size(); i++) {
            REQUIRE_EQUAL(inputPoints[i], polyPoints[i]);
        }
    }
}