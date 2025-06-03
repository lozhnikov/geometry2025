/**
 * @file methods/alg_graham_method.cpp 
 * @author Almaz Sadykov
 * 
 * @brief Implementation of the algorithm for constructing the Graham convex hull.
 */

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "../include/alg_graham.hpp"

/**
 * @brief The method of implementing the algorithm for constructing the convex hull of the Graham scan
 * 
 * @param input input data in JSON format
 * @param output pointer to JSON output
 * @return return code: 0 - success, otherwise - error
 */

namespace geometry {
  int AlgGrahamMethod(const nlohmann::json& input, nlohmann::json* output) {
    try {
      if (!input.contains("points") || !input["points"].is_array()) {
        (*output)["error"] = "Input must contain 'points' array";
        return 1;
      }

      std::vector<geometry::Point<double>> points;
      for (const auto& point_json : input["points"]) {
        if (!point_json.is_object() ||
            !point_json.contains("x") ||
            !point_json["x"].is_number()) {
          (*output)["error"] = "Each point must have 'x' numeric field";
          return 2;
        }
        if (!point_json.is_object() ||
            !point_json.contains("y") ||
            !point_json["y"].is_number()) {
          (*output)["error"] = "Each point must have 'y' numeric field";
          return 2;
        }

        points.emplace_back(
          point_json["x"].get<double>(),
          point_json["y"].get<double>());
      }

      auto convex_hull = geometry::AlgGraham(points);

      nlohmann::json hull_json = nlohmann::json::array();
      for (const auto& point : convex_hull) {
        hull_json.push_back({
          {"x", point.X()},
          {"y", point.Y()}
        });
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


 /**
  * Input JSON structure:
  * {
  *   "points" : [
  *      {"x": 0.0, "y": 0.0},
  *      {"x": 1.0, "y": 1.0},
  *      {"x": 2.0, "y": 0.0}
  *   ]
  * }
  * 
  * Output JSON structure:
  * {
  *   "convex_hull" : [
  *     {"x": 0.0, "y": 0.0},
  *     {"x": 1.0, "y": 1.0},
  *     {"x": 2.0, "y": 0.0}
  *   ],
  *   "hull_size" : 3,
  *   "original_size" : 3
  * }
  */


