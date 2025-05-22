/**
 * @file tests/my_polig_test.cpp
 * @author Nika Adzhindzhal
 */

#include <vector>
#include <random>
#include <nlohmann/json.hpp>
#include <httplib.h>
#include "test_core.hpp"
#include "test.hpp"

namespace geometry {
    struct Point {
        double x, y;
    };
} // namespace geometry

static void BasicTest(httplib::Client* cli);
static void ValidationTest(httplib::Client* cli);
static void RandomTest(httplib::Client* cli);

void TestMyPolig(httplib::Client* cli) {
    TestSuite suite("TestMyPolig");
    RUN_TEST_REMOTE(suite, cli, BasicTest);
    RUN_TEST_REMOTE(suite, cli, ValidationTest);
    RUN_TEST_REMOTE(suite, cli, RandomTest);
}

static void BasicTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "n": 5,
        "k": 2,
        "radius": 100.0
    })"_json;

    httplib::Result res = cli->Post("/My_Polig", input.dump(), "application/json");
    nlohmann::json output = nlohmann::json::parse(res->body);

    REQUIRE_EQUAL(output["vertices"].size(), 5);
    REQUIRE_EQUAL(output["edges"].size(), 5);

    REQUIRE_CLOSE(output["vertices"][0]["x"], 100.0, 1e-5);
    REQUIRE_CLOSE(output["vertices"][0]["y"], 0.0, 1e-5);
    
    std::vector<std::pair<int, int>> expected_edges = { {0,2}, {2,4}, {4,1}, {1,3}, {3,0} };
    for (size_t i = 0; i < 5; ++i) {
        REQUIRE_EQUAL(output["edges"][i]["a"], expected_edges[i].first);
        REQUIRE_EQUAL(output["edges"][i]["b"], expected_edges[i].second);
    }
}

static void ValidationTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "n": 6,
        "k": 2
    })"_json;

    httplib::Result res = cli->Post("/My_Polig", input.dump(), "application/json");
    REQUIRE_EQUAL(res->status, 400);
}

static void RandomTest(httplib::Client* cli) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> n_dist(3, 50);
    std::uniform_int_distribution<int> k_dist(1, 20);

    for (int i = 0; i < 20; ++i) {
        int n = n_dist(gen);
        int k = k_dist(gen) % (n - 1) + 1;

        while (std::gcd(n, k) != 1) {
            k = k_dist(gen) % (n - 1) + 1;
        }

        nlohmann::json input;
        input["n"] = n;
        input["k"] = k;

        httplib::Result res = cli->Post("/My_Polig", input.dump(), "application/json");
        REQUIRE_EQUAL(res->status, 200);

        nlohmann::json output = nlohmann::json::parse(res->body);

        REQUIRE_EQUAL(output["vertices"].size(), n);
        REQUIRE_EQUAL(output["edges"].size(), n);

        int current = 0;
        int count = 0;
        do {
            current = (current + k) % n;
            count++;
        } while (current != 0);

        REQUIRE_EQUAL(count, n);
    }
}