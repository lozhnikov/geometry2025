// methods/gift_wrapping_method.cpp
/**
 * @file methods/gift_wrapping_method.cpp
 * @author Ivan Kuznetsov
 *
 * @brief Gift wrapping convex hull algorithm implementation.
 */

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "gift_wrapping.hpp"

namespace geometry {

int GiftWrappingMethod(const nlohmann::json& input, nlohmann::json* output) {
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

    auto convex_hull = geometry::GiftWrapping(points);

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
