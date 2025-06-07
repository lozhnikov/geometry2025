/**
 * @file methods/alg_graham_method.cpp
 * @author Almaz Sadikov
 *
 * @brief Implementation of the Graham convex hull construction algorithm.
 */

#include <vector>
#include <string>
#include <utility>
#include <nlohmann/json.hpp>
#include "../include/alg_graham.hpp"

namespace geometry {

/**
 * @brief A method for implementing the algorithm for constructing the
 *  convex hull of the Graham method.
 *
 * @param input input data in JSON format
 * @param output pointer to JSON output
 * @return return code: 0 - success, otherwise - error
 */
int AlgGrahamMethod(const nlohmann::json& input, nlohmann::json* output) {
  try {
    // Validate input structure
    if (!input.is_object() || !input.contains("points")) {
      (*output)["error"] = "Input must contain 'points' array";
      return 1;
    }

    if (!input["points"].is_array() || input["points"].empty()) {
      (*output)["error"] = "'points' must be a non-empty array";
      return 1;
    }

    std::vector<geometry::Point<double>> points;
    points.reserve(input["points"].size());  // Pre-allocate memory

    // Parse points from JSON
    for (const auto& point_json : input["points"]) {
      if (!point_json.is_object()) {
        (*output)["error"] = "Each point must be an object";
        return 2;
      }

      if (!point_json.contains("x") || !point_json["x"].is_number() ||
          !point_json.contains("y") || !point_json["y"].is_number()) {
        (*output)["error"] = "Point must have numeric 'x' and 'y' fields";
        return 2;
      }

      points.emplace_back(point_json["x"].get<double>(),
                         point_json["y"].get<double>());
    }

    // Compute convex hull
    const auto convex_hull = geometry::AlgGraham(points);

    // Prepare JSON output
    nlohmann::json hull_json = nlohmann::json::array();
    for (const auto& point : convex_hull) {
      hull_json.push_back({{"x", point.X()}, {"y", point.Y()}});
    }

    (*output)["convex_hull"] = std::move(hull_json);
    (*output)["hull_size"] = convex_hull.size();
    (*output)["original_size"] = points.size();

    return 0;
  } catch (const std::exception& e) {
    (*output)["error"] = "Exception: " + std::string(e.what());
    return -1;
  }
}
}  // namespace geometry

 