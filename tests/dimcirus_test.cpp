/**
 * @file tests/dimcirus_test.cpp
 * @author Dmitrii Chebanov
 *
 */

#include <dimcirus.hpp>
#include <edge.hpp>
#include <polygon.hpp>
#include <httplib.h>
#include <string>
#include <vector>
#include <random>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"

namespace geometry {

    static void SimpleTestInside(httplib::Client* cli);
    static void SimpleTestOutside(httplib::Client* cli);
    static void SimpleTestIntersect(httplib::Client* cli);
    static void RandomTest(httplib::Client* cli);

    void TestDimcirus(httplib::Client* cli) {
        TestSuite suite("TestDimcirus");

        RUN_TEST_REMOTE(suite, cli, SimpleTestInside);
        RUN_TEST_REMOTE(suite, cli, SimpleTestOutside);
        RUN_TEST_REMOTE(suite, cli, SimpleTestIntersect);
        RUN_TEST_REMOTE(suite, cli, RandomTest);
    }

    static void SimpleTestInside(httplib::Client* cli) {
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


    static void SimpleTestOutside(httplib::Client* cli) {
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

    static void SimpleTestIntersect(httplib::Client* cli) {
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


    static void RandomTest(httplib::Client* cli) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(-10.0, 10.0);

        for (int i = 0; i < 10; ++i) {
            nlohmann::json polygon;
            polygon["vertices"] = {
              {{"x", 0.0}, {"y", 0.0}},
              {{"x", 5.0}, {"y", 0.0}},
              {{"x", 5.0}, {"y", 5.0}},
              {{"x", 0.0}, {"y", 5.0}}
            };

            nlohmann::json segment;
            segment["origin"]["x"] = dist(gen);
            segment["origin"]["y"] = dist(gen);
            segment["destination"]["x"] = dist(gen);
            segment["destination"]["y"] = dist(gen);

            nlohmann::json input;
            input["id"] = 100 + i;
            input["type"] = "double";
            input["segment"] = segment;
            input["polygon"] = polygon;

            httplib::Result res = cli->Post("/Dimcirus", input.dump(), "application/json");
            nlohmann::json output = nlohmann::json::parse(res->body);

            if (output["success"]) {
                double ox = output["result"]["origin"]["x"];
                double oy = output["result"]["origin"]["y"];
                REQUIRE(ox >= 0.0 && ox <= 5.0 && oy >= 0.0 && oy <= 5.0);

                double dx = output["result"]["destination"]["x"];
                double dy = output["result"]["destination"]["y"];
                REQUIRE(dx >= 0.0 && dx <= 5.0 && dy >= 0.0 && dy <= 5.0);
            }
        }
    }

} 

