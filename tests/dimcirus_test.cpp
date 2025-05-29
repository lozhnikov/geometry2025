/**
 * @file tests/dimcirus_test.cpp
 * @author Dmitrii Chebanov
 *
 * @brief Implementation of tests for Cyrus-Beck line clipping algorithm
 */

#include <httplib.h>
#include <vector>
#include <random>
#include <utility>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"
#include "dimcirus.hpp"
#include "edge.hpp"
#include "polygon.hpp"

namespace geometry {

static void SegmentInsideTest(httplib::Client* cli);
static void SegmentOutsideTest(httplib::Client* cli);
static void SegmentIntersectTest(httplib::Client* cli);
static void RandomSegmentsTest(httplib::Client* cli);

void TestDimcirus(httplib::Client* cli) {
    TestSuite suite("TestDimcirus");

    RUN_TEST_REMOTE(suite, cli, SegmentInsideTest);
    RUN_TEST_REMOTE(suite, cli, SegmentOutsideTest);
    RUN_TEST_REMOTE(suite, cli, SegmentIntersectTest);
    RUN_TEST_REMOTE(suite, cli, RandomSegmentsTest);
}

/**
 * @brief Test with segment completely inside the polygon
 *
 * @param cli pointer to HTTP client
 */
static void SegmentInsideTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "id": 1,
        "type": "float",
        "segment": {
            "origin": { "x": 1.0, "y": 1.0 },
            "destination": { "x": 2.0, "y": 2.0 }
        },
        "polygon": {
            "vertices": [
                { "x": 0.0, "y": 0.0 },
                { "x": 3.0, "y": 0.0 },
                { "x": 3.0, "y": 3.0 },
                { "x": 0.0, "y": 3.0 }
            ]
        }
    })"_json;

    httplib::Result res = cli->Post("/Dimcirus", input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(true, output["success"]);
    REQUIRE_EQUAL(1, output["id"]);
    REQUIRE_EQUAL("float", output["type"]);

    REQUIRE_EQUAL(1.0f, output["result"]["origin"]["x"]);
    REQUIRE_EQUAL(1.0f, output["result"]["origin"]["y"]);
    REQUIRE_EQUAL(2.0f, output["result"]["destination"]["x"]);
    REQUIRE_EQUAL(2.0f, output["result"]["destination"]["y"]);
}

/**
 * @brief Test with segment completely outside the polygon
 *
 * @param cli pointer to HTTP client
 */
static void SegmentOutsideTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "id": 2,
        "type": "float",
        "segment": {
            "origin": { "x": -1.0, "y": -1.0 },
            "destination": { "x": -2.0, "y": -2.0 }
        },
        "polygon": {
            "vertices": [
                { "x": 0.0, "y": 0.0 },
                { "x": 3.0, "y": 0.0 },
                { "x": 3.0, "y": 3.0 },
                { "x": 0.0, "y": 3.0 }
            ]
        }
    })"_json;

    httplib::Result res = cli->Post("/Dimcirus", input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(false, output["success"]);
    REQUIRE_EQUAL(2, output["id"]);
}

/**
 * @brief Test with segment intersecting the polygon
 *
 * @param cli pointer to HTTP client
 */
static void SegmentIntersectTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "id": 3,
        "type": "double",
        "segment": {
            "origin": { "x": -1.0, "y": 1.0 },
            "destination": { "x": 4.0, "y": 1.0 }
        },
        "polygon": {
            "vertices": [
                { "x": 0.0, "y": 0.0 },
                { "x": 3.0, "y": 0.0 },
                { "x": 3.0, "y": 3.0 },
                { "x": 0.0, "y": 3.0 }
            ]
        }
    })"_json;

    httplib::Result res = cli->Post("/Dimcirus", input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(true, output["success"]);
    REQUIRE_EQUAL(3, output["id"]);
    REQUIRE_EQUAL("double", output["type"]);

    REQUIRE_EQUAL(0.0, output["result"]["origin"]["x"]);
    REQUIRE_EQUAL(1.0, output["result"]["origin"]["y"]);
    REQUIRE_EQUAL(3.0, output["result"]["destination"]["x"]);
    REQUIRE_EQUAL(1.0, output["result"]["destination"]["y"]);
}

/**
 * @brief Random test with segment clipping properties checking
 *
 * @param cli pointer to HTTP client
 */
static void RandomSegmentsTest(httplib::Client* cli) {
    const int numTests = 50;
    const double eps = 1e-6;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    for (int i = 0; i < numTests; ++i) {
        nlohmann::json input;
        input["id"] = 100 + i;
        input["type"] = "double";
        
        // Define a square polygon
        input["polygon"]["vertices"] = {
            {{"x", 0.0}, {"y", 0.0}},
            {{"x", 5.0}, {"y", 0.0}},
            {{"x", 5.0}, {"y", 5.0}},
            {{"x", 0.0}, {"y", 5.0}}
        };

        // Generate random segment
        input["segment"]["origin"]["x"] = dist(gen);
        input["segment"]["origin"]["y"] = dist(gen);
        input["segment"]["destination"]["x"] = dist(gen);
        input["segment"]["destination"]["y"] = dist(gen);

        httplib::Result res = cli->Post("/Dimcirus", input.dump(), "application/json");
        nlohmann::json output = nlohmann::json::parse(res->body);

        if (output["success"]) {
            double ox = output["result"]["origin"]["x"];
            double oy = output["result"]["origin"]["y"];
            REQUIRE(ox >= 0.0 - eps && ox <= 5.0 + eps);
            REQUIRE(oy >= 0.0 - eps && oy <= 5.0 + eps);

            double dx = output["result"]["destination"]["x"];
            double dy = output["result"]["destination"]["y"];
            REQUIRE(dx >= 0.0 - eps && dx <= 5.0 + eps);
            REQUIRE(dy >= 0.0 - eps && dy <= 5.0 + eps);
        }
    }
}

}  // namespace geometry
