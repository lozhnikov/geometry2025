/**
 * @file methods/sutherland_hodgman_method.cpp 
 * @brief Sutherland-Hodgman polygon clipping algorithm implementation.
 */

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "../include/sutherland_hodgman.hpp"

/**
 * @brief Method for Sutherland-Hodgman polygon clipping.
 * 
 * @param input input data in JSON format
 * @param output pointer to JSON output
 * @return return code: 0 - success, otherwise - error
 */
namespace geometry {
int SutherlandHodgmanMethod(const nlohmann::json& input,
                           nlohmann::json* output) {
  try {
    // Validate input structure
    if (!input.contains("subject_polygon") ||
        !input["subject_polygon"].is_array()) {
      (*output)["error"] = "Input must contain 'subject_polygon' array";
      return 1;
    }
    if (!input.contains("clip_polygon") ||
        !input["clip_polygon"].is_array()) {
      (*output)["error"] = "Input must contain 'clip_polygon' array";
      return 1;
    }

    std::vector<geometry::Point<double>> subject_polygon, clip_polygon;
    
    // Parse subject polygon
    for (const auto& point_json : input["subject_polygon"]) {
      if (!point_json.is_object() ||
          !point_json.contains("x") ||
          !point_json["x"].is_number()) {
        (*output)["error"] = "Each point must have 'x' numeric field";
        return 2;
      }
      if (!point_json.contains("y") ||
          !point_json["y"].is_number()) {
        (*output)["error"] = "Each point must have 'y' numeric field";
        return 2;
      }

      subject_polygon.emplace_back(
        point_json["x"].get<double>(),
        point_json["y"].get<double>());
    }

    // Parse clip polygon
    for (const auto& point_json : input["clip_polygon"]) {
      if (!point_json.is_object() ||
          !point_json.contains("x") ||
          !point_json["x"].is_number()) {
        (*output)["error"] = "Each point must have 'x' numeric field";
        return 2;
      }
      if (!point_json.contains("y") ||
          !point_json["y"].is_number()) {
        (*output)["error"] = "Each point must have 'y' numeric field";
        return 2;
      }

      clip_polygon.emplace_back(
        point_json["x"].get<double>(),
        point_json["y"].get<double>());
    }

    // Perform clipping
    auto result = geometry::SutherlandHodgman(subject_polygon, clip_polygon);

    // Prepare output
    nlohmann::json result_json = nlohmann::json::array();
    for (const auto& point : result) {
      result_json.push_back({
        {"x", point.X()},
        {"y", point.Y()}
      });
    }

    (*output)["result"] = result_json;
    (*output)["result_size"] = result.size();
    (*output)["subject_size"] = subject_polygon.size();
    (*output)["clip_size"] = clip_polygon.size();

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
 *   "subject_polygon": [
 *     {"x": 0.0, "y": 0.0},
 *     {"x": 1.0, "y": 0.0},
 *     {"x": 1.0, "y": 1.0},
 *     {"x": 0.0, "y": 1.0}
 *   ],
 *   "clip_polygon": [
 *     {"x": 0.5, "y": 0.5},
 *     {"x": 1.5, "y": 0.5},
 *     {"x": 1.5, "y": 1.5},
 *     {"x": 0.5, "y": 1.5}
 *   ]
 * }
 * 
 * Output JSON structure:
 * {
 *   "result": [
 *     {"x": 0.5, "y": 0.5},
 *     {"x": 1.0, "y": 0.5},
 *     {"x": 1.0, "y": 1.0},
 *     {"x": 0.5, "y": 1.0}
 *   ],
 *   "result_size": 4,
 *   "subject_size": 4,
 *   "clip_size": 4
 * }
 */
