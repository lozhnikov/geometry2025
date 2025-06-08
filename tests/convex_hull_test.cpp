/**
 * @file tests/convex_hull_test.cpp 
 * @author Almaz Sadikov
 * 
 * @brief Tests for convex hull computation using Graham's  algorithm
 */
#include <httplib.h>
#include <vector>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"
static void SimpleHullTest(httplib::Client* cli);
static void CollinearTest(httplib::Client* cli);
static void RandomHullTest(httplib::Client* cli);
void TestConvexHull(httplib::Client* cli) {
    TestSuite suite("TestConvexHull");
    // Basic convex hull test with simple square shape
    RUN_TEST_REMOTE(suite, cli, SimpleHullTest);
    // Test handling of collinear points
    RUN_TEST_REMOTE(suite, cli, CollinearTest);
    // Randomized stress testing
    RUN_TEST_REMOTE(suite, cli, RandomHullTest);
}
/**
 * @brief Test with a simple square convex hull
 */
static void SimpleHullTest(httplib::Client* cli) {
    // Input: Square with 4 vertices + internal point
    nlohmann::json input = {
        {"points", {
            {{"x", 0.0}, {"y", 0.0}},  // Vertex
            {{"x", 1.0}, {"y", 0.0}},  // Vertex
            {{"x", 1.0}, {"y", 1.0}},  // Vertex
            {{"x", 0.0}, {"y", 1.0}},  // Vertex
            {{"x", 0.5}, {"y", 0.5}}   // Internal point (should be excluded)
        }}
    };

    auto res = cli->Post("/ComputeConvexHull", input.dump(),
     "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    // Verify only the 4 vertices are in hull
    REQUIRE_EQUAL(4, output["hull_size"]);
    // All 5 input points should be processed
    REQUIRE_EQUAL(5, output["input_size"]);
}
/**
 * @brief Test with collinear points
 */
static void CollinearTest(httplib::Client* cli) {
    // Input: Horizontal line + two non-collinear points
    nlohmann::json input = {
        {"points", {
            {{"x", 0.0}, {"y", 0.0}},  // Start point
            {{"x", 1.0}, {"y", 0.0}},  // Middle (should be excluded)
            {{"x", 2.0}, {"y", 0.0}},  // End point
            {{"x", 0.0}, {"y", 1.0}},  // Non-collinear
            {{"x", 1.0}, {"y", 1.0}}   // Non-collinear
        }}
    };
    auto res = cli->Post("/ComputeConvexHull", input.dump(),
     "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);
    // Should keep 4 points: two extremes of line + two non-collinear
    REQUIRE_EQUAL(4, output["hull_size"]);
    REQUIRE_EQUAL(5, output["input_size"]);
}
/**
 * @brief Randomized convex hull test
 */
static void RandomHullTest(httplib::Client* cli) {
    // Setup random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-10.0, 10.0);
    // Generate random input
    nlohmann::json input;
    const int num_points = 20;
    for (int i = 0; i < num_points; i++) {
        input["points"][i]["x"] = dis(gen);
        input["points"][i]["y"] = dis(gen);
    }
    auto res = cli->Post("/ComputeConvexHull", input.dump(),
     "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);
    // Basic verification
    REQUIRE_EQUAL(num_points, output["input_size"]);
    REQUIRE(output["hull_size"] <= output["input_size"]);
}
