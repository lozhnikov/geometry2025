/**
 * @file tests/cyrus_beck_test.cpp
 *
 * @brief Tests for Cyrus-Beck line clipping algorithm.
 */

#include <httplib.h>
#include <vector>
#include <random>
#include <utility>
#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"

static void SimpleClipTest(httplib::Client* cli);
static void OutsideClipTest(httplib::Client* cli);
static void RandomClipTest(httplib::Client* cli);

void TestCyrusBeck(httplib::Client* cli) {
    TestSuite suite("TestCyrusBeck");

    RUN_TEST_REMOTE(suite, cli, SimpleClipTest);
    RUN_TEST_REMOTE(suite, cli, OutsideClipTest);
    RUN_TEST_REMOTE(suite, cli, RandomClipTest);
}

static void SimpleClipTest(httplib::Client* cli) {
    nlohmann::json input = R"(
        {
            "segment": {
                "start": {"x": 0.0, "y": 0.0},
                "end": {"x": 6.0, "y": 3.0}
            },
            "polygon": [
                {"x": 2.0, "y": 0.0},
                {"x": 2.0, "y": 2.0},
                {"x": 5.0, "y": 2.0},
                {"x": 5.0, "y": 0.0}
            ]
        }
    )"_json;

    httplib::Result res = cli->Post("/CyrusBek",
                                   input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(true, output["visible"]);
    REQUIRE(output.contains("clipped_segment"));
}

static void OutsideClipTest(httplib::Client* cli) {
    nlohmann::json input = R"(
        {
            "segment": {
                "start": {"x": -1.0, "y": -1.0},
                "end": {"x": -2.0, "y": -2.0}
            },
            "polygon": [
                {"x": 1.0, "y": 0.0},
                {"x": 2.0, "y": 1.0},
                {"x": 1.0, "y": 2.0},
                {"x": 0.0, "y": 1.0}
            ]
        }
    )"_json;

    httplib::Result res = cli->Post("/CyrusBek",
                                   input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(false, output["visible"]);
}

static void RandomClipTest(httplib::Client* cli) {
    const int numTries = 50;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> coordDist(-10.0, 10.0);
    for (int i = 0; i < numTries; i++) {
        std::vector<double> angles;
        for (int j = 0; j < 10; j++) {
            angles.push_back(coordDist(gen));
        }
        std::sort(angles.begin(), angles.end());
        nlohmann::json input;
        for (double angle : angles) {
            double x = 5.0 * cos(angle);
            double y = 5.0 * sin(angle);
            input["polygon"].push_back({{"x", x}, {"y", y}});
        }
        input["segment"]["start"]["x"] = coordDist(gen);
        input["segment"]["start"]["y"] = coordDist(gen);
        input["segment"]["end"]["x"] = coordDist(gen);
        input["segment"]["end"]["y"] = coordDist(gen);
        
        httplib::Result res = cli->Post("/CyrusBek",
                                    input.dump(), "application/json");
        nlohmann::json output = nlohmann::json::parse(res->body);
        
        REQUIRE(output.contains("visible"));
        if (output["visible"]) {
          REQUIRE(output.contains("clipped_segment"));
        }
    }
}
