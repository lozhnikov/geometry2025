/**
 * @file methods/angle_point_in_polygon_method.cpp 
 * @author Your Name
 * 
 * @brief Angle-based point-in-polygon algorithm implementation.
 */

#include <vector>
#include <list>
#include <string>
#include <nlohmann/json.hpp>
#include "../include/angle_point_in_polygon.hpp"

/**
 * @brief Method for angle-based point-in-polygon algorithm implementation.
 * 
 * @param input input data in JSON format
 * @param output pointer to JSON output
 * @return return code: 0 - success, otherwise - error
 */
namespace geometry {
int AnglePointInPolygonMethod(const nlohmann::json& input, \
     nlohmann::json* output) {
    try {
        // Validate input
        if (!input.contains("point") || !input["point"].is_object()) {
            (*output)["error"] = "Input must contain 'point' object";
            return 1;
        }
        if (!input.contains("polygon") || !input["polygon"].is_array()) {
            (*output)["error"] = "Input must contain 'polygon' array";
            return 2;
        }
        // Parse point
        if (!input["point"].contains("x") || !input["point"]["x"].is_number()) {
            (*output)["error"] = "Point must have 'x' numeric field";
            return 3;
        }
        if (!input["point"].contains("y") || !input["point"]["y"].is_number()) {
            (*output)["error"] = "Point must have 'y' numeric field";
            return 4;
        }
        Point<double> point(
            input["point"]["x"].get<double>(),
            input["point"]["y"].get<double>());
        // Parse polygon
        std::list<Point<double>> polygon_points;
        for (const auto& point_json : input["polygon"]) {
            if (!point_json.is_object() ||
                !point_json.contains("x") || !point_json["x"].is_number() ||
                !point_json.contains("y") || !point_json["y"].is_number()) {
                (*output)["error"] = "Each polygon point must have 'x' & 'y'";
                return 5;
            }
            polygon_points.emplace_back(\
                point_json["x"].get<double>(), \
                point_json["y"].get<double>());
        }
        Polygon<double> polygon(polygon_points);
        // Run algorithm with default precision
        auto position = AnglePointInPolygon(point, polygon, 1e-9);
        // Prepare output
        switch (position) {
            case PointPosition::INSIDE:
                (*output)["position"] = "inside";
                break;
            case PointPosition::OUTSIDE:
                (*output)["position"] = "outside";
                break;
            case PointPosition::BOUNDARY:
                (*output)["position"] = "boundary";
                break;
        }
        (*output)["point"] = {
            {"x", point.X()},
            {"y", point.Y()}
        };
        (*output)["polygon_size"] = polygon.Size();
        return 0;
    } catch (const std::exception& e) {
        (*output)["error"] = std::string("Exception: ") + e.what();
        return -1;
    }
}
}  // namespace geometry
