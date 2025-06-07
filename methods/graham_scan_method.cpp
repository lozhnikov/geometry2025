/**
 * @file methods/graham_scan_method.cpp
 * @author Maria Fedorova
 *
 * @brief Graham scan convex hull algorithm implementation.
 */

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "../include/graham_scan.hpp"

namespace geometry {

/**
 * @brief Method for Graham scan convex hull algorithm implementation.
 *
 * @param input input data in JSON format
 * @param output pointer to JSON output
 * @return return code: 0 - success, otherwise - error
 */
int GrahamScanMethod(const nlohmann::json& input, nlohmann::json* output) {
  try {
    if (!input.contains("points") || !input["points"].is_array()) {
      (*output)["error"] = "Input must contain 'points' array";
      return 1;
    }

    std::vector<geometry::Point<double>> points;
    for (const auto& point_json : input["points"]) {
      if (!point_json.is_object() ||
          !point_json.contains("x") ||
          !point_json["x"].is_number() ||
          !point_json.contains("y") ||
          !point_json["y"].is_number()) {
        (*output)["error"] = "Each point must have numeric 'x' and 'y' fields";
        return 2;
      }

      points.emplace_back(
          point_json["x"].get<double>(),
          point_json["y"].get<double>());
    }

    auto convex_hull = geometry::GrahamScan(points);

    nlohmann::json hull_json = nlohmann::json::array();
    for (const auto& point : convex_hull) {
      hull_json.push_back({{"x", point.X()}, {"y", point.Y()}});
    }

    (*output)["convex_hull"] = hull_json;
    (*output)["hull_size"] = convex_hull.size();
    (*output)["original_size"] = points.size();

    return 0;
  } catch (const std::exception& e) {
    (*output)["error"] = std::string("Exception: ") + e.what();
    return -1;
  }
}
}  // namespace geometry

// Пример JSON (оставлен как комментарий)
// {
//   "points": [
//     {"x": 0.0, "y": 0.0},
//     {"x": 1.0, "y": 1.0},
//     {"x": 2.0, "y": 0.0}
//   ]
// }