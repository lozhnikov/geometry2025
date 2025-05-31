/**
 * @file methods/closest_pair_method.cpp
 * @brief Closest Pair of Points algorithm JSON interface.
 */

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "../include/closest_pair_impl.hpp"

/**
 * @brief Method for computing the closest pair of points using JSON input/output.
 *
 * @param input JSON containing "points" array
 * @param output pointer to JSON where result or error will be stored
 * @return 0 on success, non-zero on failure
 */

namespace geometry {

int ClosestPairMethod(const nlohmann::json& input, nlohmann::json* output) {
  try {
    if (!input.contains("points") || !input["points"].is_array()) {
      (*output)["error"] = "Input must contain 'points' array";
      return 1;
    }

    std::vector<Point<double>> points;

    for (const auto& pt : input["points"]) {
      if (!pt.is_object() || !pt.contains("x") || !pt.contains("y") ||
          !pt["x"].is_number() || !pt["y"].is_number()) {
        (*output)["error"] = "Each point must be an object with numeric 'x' and 'y'";
        return 2;
      }
      points.emplace_back(pt["x"].get<double>(), pt["y"].get<double>());
    }

    if (points.size() < 2) {
      (*output)["error"] = "At least 2 points are required";
      return 3;
    }

    Edge<double> closestPair;
    double distance = ClosestPair(points, closestPair);

    (*output)["point1"] = {{"x", closestPair.p1.x}, {"y", closestPair.p1.y}};
    (*output)["point2"] = {{"x", closestPair.p2.x}, {"y", closestPair.p2.y}};
    (*output)["distance"] = distance;
    (*output)["input_size"] = points.size();

    return 0;

  } catch (const std::exception& e) {
    (*output)["error"] = std::string("Exception: ") + e.what();
    return -1;
  }
}

}  // namespace geometry

/**
 * 📥 Example input:
 * {
 *   "points": [
 *     {"x": 0.0, "y": 0.0},
 *     {"x": 1.0, "y": 1.0},
 *     {"x": 3.0, "y": 4.0}
 *   ]
 * }
 *
 * 📤 Example output:
 * {
 *   "point1": {"x": 0.0, "y": 0.0},
 *   "point2": {"x": 1.0, "y": 1.0},
 *   "distance": 1.4142135,
 *   "input_size": 3
 * }
 */
