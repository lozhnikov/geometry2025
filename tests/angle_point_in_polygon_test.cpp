
/**
 * @file tests/angle_point_in_polygon_test.cpp 
 * @author Your Name
 * 
 * @brief Realization of a set of tests for the angle-based point-in-polygon algorithm
 */

#include <httplib.h>
#include <vector>
#include <random>
#include <utility>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"

static void SimpleInsideTest(httplib::Client* cli);
static void SimpleOutsideTest(httplib::Client* cli);
static void BoundaryTest(httplib::Client* cli);
static void RandomPointsTest(httplib::Client* cli);

void TestAnglePointInPolygon(httplib::Client* cli) {
    TestSuite suite("TestAnglePointInPolygon");

    RUN_TEST_REMOTE(suite, cli, SimpleInsideTest);
    RUN_TEST_REMOTE(suite, cli, SimpleOutsideTest);
    RUN_TEST_REMOTE(suite, cli, BoundaryTest);
    RUN_TEST_REMOTE(suite, cli, RandomPointsTest);
}

/**
 * @brief Test with a point clearly inside a square polygon
 */
static void SimpleInsideTest(httplib::Client* cli) {
    nlohmann::json input = R"(
        {
            "point": {"x": 0.5, "y": 0.5},
            "polygon": [
                {"x": 0.0, "y": 0.0},
                {"x": 1.0, "y": 0.0},
                {"x": 1.0, "y": 1.0},
                {"x": 0.0, "y": 1.0}
            ]
        }
    )"_json;

    httplib::Result res = cli->Post("/AnglePointInPolygon",
                                  input.dump(), "application/json");

    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL("inside", output["position"]);
    REQUIRE_EQUAL(4, output["polygon_size"]);
}

/**
 * @brief Test with a point clearly outside a square polygon
 */
static void SimpleOutsideTest(httplib::Client* cli) {
    nlohmann::json input = R"(
        {
            "point": {"x": 1.5, "y": 0.5},
            "polygon": [
                {"x": 0.0, "y": 0.0},
                {"x": 1.0, "y": 0.0},
                {"x": 1.0, "y": 1.0},
                {"x": 0.0, "y": 1.0}
            ]
        }
    )"_json;

    httplib::Result res = cli->Post("/AnglePointInPolygon",
                                  input.dump(), "application/json");

    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL("outside", output["position"]);
    REQUIRE_EQUAL(4, output["polygon_size"]);
}

/**
 * @brief Test with a point on the boundary of a square polygon
 */
static void BoundaryTest(httplib::Client* cli) {
    nlohmann::json input = R"(
        {
            "point": {"x": 0.5, "y": 0.0},
            "polygon": [
                {"x": 0.0, "y": 0.0},
                {"x": 1.0, "y": 0.0},
                {"x": 1.0, "y": 1.0},
                {"x": 0.0, "y": 1.0}
            ]
        }
    )"_json;

    httplib::Result res = cli->Post("/AnglePointInPolygon",
                                  input.dump(), "application/json");

    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL("boundary", output["position"]);
    REQUIRE_EQUAL(4, output["polygon_size"]);
}

/**
 * @brief Random test with the properties of point-in-polygon checking
 */
static void RandomPointsTest(httplib::Client* cli) {
    const int numTries = 50;
    const int maxPoints = 20;
    const double range = 100.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> sizeDist(3, maxPoints);
    std::uniform_real_distribution<double> coordDist(-range, range);

    for (int it = 0; it < numTries; it++) {
        // Generate convex polygon (to simplify testing)
        size_t size = sizeDist(gen);
        std::vector<Point<double>> points;
        // Generate points around a circle
        double center_x = coordDist(gen);
        double center_y = coordDist(gen);
        double radius = coordDist(gen) / 2.0;
        if (radius < 0) radius = -radius;
        for (size_t i = 0; i < size; i++) {
            double angle = 2 * M_PI * i / size;
            points.emplace_back(
                center_x + radius * cos(angle),
                center_y + radius * sin(angle));
        }
        // Generate test point
        Point<double> test_point(
            center_x + (coordDist(gen) / range) * radius,
            center_y + (coordDist(gen) / range) * radius);
        // Prepare input
        nlohmann::json input;
        input["point"]["x"] = test_point.X();
        input["point"]["y"] = test_point.Y();
        for (size_t i = 0; i < points.size(); i++) {
            input["polygon"][i]["x"] = points[i].X();
            input["polygon"][i]["y"] = points[i].Y();
        }
        // Call method
        httplib::Result res = cli->Post("/AnglePointInPolygon",
                                      input.dump(), "application/json");
        nlohmann::json output = nlohmann::json::parse(res->body);
        // Verify output
        REQUIRE_EQUAL(size, output["polygon_size"]);
        // Simple verification - if point is within radius it should be inside
        double distance = sqrt(pow(test_point.X() - center_x, 2) + 
                              pow(test_point.Y() - center_y, 2));
        if (distance < radius * 0.99) {
            REQUIRE_EQUAL("inside", output["position"]);
        } else if (distance > radius * 1.01) {
            REQUIRE_EQUAL("outside", output["position"]);
        }
        // For points very close to boundary we don't check due to floating\
         point precision
    }
}
