#include <httplib.h>
#include <nlohmann/json.hpp>
#include "test.hpp"
#include "test_core.hpp"

static void SimpleIntersectionTest(httplib::Client* cli);

void TestSutherlandHodgman(httplib::Client* cli) {
  TestSuite suite("TestSutherlandHodgman");
  RUN_TEST_REMOTE(suite, cli, SimpleIntersectionTest);
}

static void SimpleIntersectionTest(httplib::Client* cli) {
  nlohmann::json input = {
    {"subject", {
      {{"x", 1.0}, {"y", 1.0}},
      {{"x", 4.0}, {"y", 1.0}},
      {{"x", 4.0}, {"y", 4.0}},
      {{"x", 1.0}, {"y", 4.0}}
    }},
    {"clip", {
      {{"x", 2.0}, {"y", 2.0}},
      {{"x", 5.0}, {"y", 2.0}},
      {{"x", 5.0}, {"y", 5.0}},
      {{"x", 2.0}, {"y", 5.0}}
    }}
  };

  auto res = cli->Post("/SutherlandHodgman", input.dump(), "application/json");
  nlohmann::json output = nlohmann::json::parse(res->body);

  REQUIRE(output.contains("intersection"));
  REQUIRE(output["result_size"] > 0);
}
