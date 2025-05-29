/**
 * @file methods/check_convex_method.cpp
 * @author Daria Slepenkova
 *
 * The file contains a function that calls the convex vertex recognition algorithm.
 * The function accepts and returns data in JSON format.
 */

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "../include/check_convex.hpp"

/**
 * @brief Convex vertex recognition algorithm.
 *
 * @param input input data in JSON format
 * @param output pointer to JSON output
 * @return return code: 0 - success, otherwise - error
 */

namespace geometry {
    int CheckConvex(const nlohmann::json& input, nlohmann::json* output) {
    try {
      if (!input.contains("points") || !input["points"].is_array()) {
        (*output)["error"] = "Input must contain 'points' array";
        return -1;
      }

      std::vector<geometry::Point<double>> points;
      for (const auto& point_json : input["points"]) {
        if (!point_json.is_object() ||
            !point_json.contains("x") ||
            !point_json["x"].is_number()) {
          (*output)["error"] = "Each point must have 'x' numeric field";
          return -2;
        }
        if (!point_json.is_object() ||
            !point_json.contains("y") ||
            !point_json["y"].is_number()) {
          (*output)["error"] = "Each point must have 'y' numeric field";
          return -2;
        }

        points.emplace_back(
          point_json["x"].get<double>(),
          point_json["y"].get<double>());
      }

      auto convex_points = geometry::CheckConvex(points);

      nlohmann::json convex_json = nlohmann::json::array();
      for (const auto& point : convex_points) {
        convex_json.push_back({
          {"x", point.X()},
          {"y", point.Y()}
        });
      }

      (*output)["convex_points"] = convex_json;
      (*output)["number_of_points"] = convex_points.size();

      return 0;
    } catch (const std::exception& e) {
      (*output)["error"] = std::string("Exception: ") + e.what();
      return -3;
    }
  }
}  // namespace geometry


/**
  * Input JSON structure:
  * {
  *   "points" : [
  *      {"x": 0.0, "y": 0.0},
  *      {"x": 0.0, "y": 1.0},
  *      {"x": 1.0, "y": 1.0},
  *      {"x": 1.0, "y": 0.0}
  *   ]
  * }
  *
  * Output JSON structure:
  * {
  *   "convex_points" : [
  *      {"x": 0.0, "y": 0.0},
  *      {"x": 0.0, "y": 1.0},
  *      {"x": 1.0, "y": 1.0},
  *      {"x": 1.0, "y": 0.0}
  *   ]
  *    "number_of_points" : 4
  * }
  */
