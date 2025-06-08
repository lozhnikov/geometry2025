/**
 * @file methods/angle_point_in_polygon_method.cpp 
 * @author Almaz Sadikov
 * 
 * @brief implementation of the algorithm for constructing a convex 
 * hull by the Graham method.
*/

#include <vector>
#include <nlohmann/json.hpp>
#include "../include/convex_hull_graham.hpp"

namespace geometry {

int ComputeConvexHullMethod(const nlohmann::json& input,
nlohmann::json* output) {
  try {
    // Validate input
    if (!input.contains("points") || !input["points"].is_array()) {
      (*output)["error"] = "Input must contain 'points' array";
      return 1;
    }

    // Parse points
    std::vector<Point<double>> points;
    for (const auto& point_json : input["points"]) {
      if (!point_json.is_object() ||
        !point_json.contains("x") || !point_json["x"].is_number() ||
        !point_json.contains("y") || !point_json["y"].is_number()) {
        (*output)["error"] = "Each point must have 'x' and 'y' numeric fields";
        return 2;
      }
      points.emplace_back(
        point_json["x"].get<double>(),
        point_json["y"].get<double>());
    }

    // Run algorithm
    auto hull = ComputeConvexHull(points);

    // Prepare output
    (*output)["hull_size"] = hull.size();
    for (size_t i = 0; i < hull.size(); i++) {
      (*output)["hull"][i]["x"] = hull[i].X();
        (*output)["hull"][i]["y"] = hull[i].Y();
    }
    (*output)["input_size"] = points.size();

    return 0;
  } catch (const std::exception& e) {
      (*output)["error"] = std::string("Exception: ") + e.what();
    return -1;
  }
}
}  // namespace geometry
