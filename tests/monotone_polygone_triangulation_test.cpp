/**
 * @file tests/monotonePolygonTriangulationTest.cpp
 * @author Taisiya Osipova
 *
 * @brief Realization of a set of tests for the Monotone Polygon Triangulation algorithm
 */

#include <httplib.h>
#include <vector>
#include <random>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"

static void SimpleMonotoneTest(httplib::Client* cli);
static void SquareTest(httplib::Client* cli);
static void RandomMonotoneTest(httplib::Client* cli);

void TestMonotonePolygonTriangulation(httplib::Client* cli) {
    TestSuite suite("TestMonotonePolygonTriangulation");

    RUN_TEST_REMOTE(suite, cli, SimpleMonotoneTest);
    RUN_TEST_REMOTE(suite, cli, SquareTest);
    RUN_TEST_REMOTE(suite, cli, RandomMonotoneTest);
}

/**
 * @brief Simple test with a monotone polygon (triangle)
 *
 * @param cli pointer to HTTP client
 */
static void SimpleMonotoneTest(httplib::Client* cli) {
    nlohmann::json input = R"(
        {
            "polygon" : [
                {"x": 0.0, "y": 0.0},
                {"x": 1.0, "y": 2.0},
                {"x": 2.0, "y": 0.0}
            ]
        }
    )"_json;

    httplib::Result res = cli->Post("/MonotonePolygonTriangulation", input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(0, output["diagonals_count"]);
    REQUIRE_EQUAL(3, output["vertices_count"]);
}

/**
 * @brief Test with a square (monotone polygon)
 *
 * @param cli pointer to HTTP client
 */
static void SquareTest(httplib::Client* cli) {
    nlohmann::json input = R"(
        {
            "polygon" : [
                {"x": 0.0, "y": 2.0},
                {"x": 2.0, "y": 0.0},
                {"x": 0.0, "y": -2.0},
                {"x": -2.0, "y": 0.0}
            ]
        }
    )"_json;

    httplib::Result res = cli->Post("/MonotonePolygonTriangulation", input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(1, output["diagonals_count"]);
    REQUIRE_EQUAL(4, output["vertices_count"]);

    bool valid_diagonal = false;
    for (const auto& diagonal : output["diagonals"]) {
        int from = diagonal["from"];
        int to = diagonal["to"];
        if ((from == 0 && to == 2) || (from == 2 && to == 0) ||
            (from == 1 && to == 3) || (from == 3 && to == 1)) {
            valid_diagonal = true;
        }
    }
    REQUIRE(valid_diagonal);
}

/**
 * @brief Random test with generated monotone polygons
 *
 * @param cli pointer to HTTP client
 */
static void RandomMonotoneTest(httplib::Client* cli) {
    const int numTries = 20;
    const int minVertices = 3;
    const int maxVertices = 50;
    const double minCoord = -100.0;
    const double maxCoord = 100.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> sizeDist(minVertices, maxVertices);
    std::uniform_real_distribution<double> coordDist(minCoord, maxCoord);

    for (int it = 0; it < numTries; it++) {
        size_t size = sizeDist(gen);
        nlohmann::json input;

        std::vector<double> xCoords(size);
        std::vector<double> yCoords(size);
        
        for (size_t i = 0; i < size; i++) {
            xCoords[i] = coordDist(gen);
            yCoords[i] = coordDist(gen);
        }

        // сортируем по х-координате
        std::sort(xCoords.begin(), xCoords.end());

        // создаем две цепи
        for (size_t i = 0; i < size; i++) {
            if (i < size / 2) {
                input["polygon"][i]["x"] = xCoords[i];
                input["polygon"][i]["y"] = coordDist(gen) + 50.0;
            }
            else {
                size_t reverse_idx = size - 1 - (i - size / 2);
                input["polygon"][i]["x"] = xCoords[reverse_idx];
                input["polygon"][i]["y"] = coordDist(gen) - 50.0;
            }
        }

        httplib::Result res = cli->Post("/MonotonePolygonTriangulation", input.dump(), "application/json");
        nlohmann::json output = nlohmann::json::parse(res->body);

        REQUIRE_EQUAL(size, output["vertices_count"]);
        
        // кол-во диагоналей должно быть n-3
        if (size > 3) {
            REQUIRE_EQUAL(size - 3, output["diagonals_count"]);
        } else {
            REQUIRE_EQUAL(0, output["diagonals_count"]);
        }

        for (const auto& diagonal : output["diagonals"]) {
            int from = diagonal["from"];
            int to = diagonal["to"];
            REQUIRE(from >= 0);
            REQUIRE(from < static_cast<int>(size));
            REQUIRE(to >= 0);
            REQUIRE(to < static_cast<int>(size));
            REQUIRE(from != to);
        }
    }
}
