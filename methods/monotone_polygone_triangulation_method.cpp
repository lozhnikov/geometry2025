/**
 * @file methods/monotone_polygone_triangulation_method.cpp
 * @author Taisiya Osipova
 *
 * @brief Monotone polygon triangulation algorithm implementation.
 */

#include "methods/monotone_polygone_triangulation_method.hpp"
#include <vector>
#include <string>
#include "../include/monotone_polygone_triangulation.hpp"
#include <nlohmann/json.hpp>

/**
 * @brief Method for monotone polygon triangulation algorithm implementation.
 *
 * @param input input data in JSON format
 * @param output pointer to JSON output
 * @return return code: 0 - success, otherwise - error
 */
namespace geometry {

int MonotonePolygonTriangulationMethod(const nlohmann::json& input,
                                      nlohmann::json* output) {
    try {
        if (!input.contains("polygon") || !input["polygon"].is_array()) {
            (*output)["error"] = "Input must contain 'polygon' array";
            return 1;
        }

        std::vector<Point> polygon;
        int id_counter = 0;
        
        for (const auto& point_json : input["polygon"]) {
            if (!point_json.is_object() || !point_json.contains("x") ||
                !point_json["x"].is_number()) {
                (*output)["error"] = "Each point must have 'x' numeric field";
                return 2;
            }
            if (!point_json.is_object() || !point_json.contains("y") ||
                !point_json["y"].is_number()) {
                (*output)["error"] = "Each point must have 'y' numeric field";
                return 2;
            }

            polygon.emplace_back(
                point_json["x"].get<double>(),
                point_json["y"].get<double>(),
                id_counter++);
        }

        if (polygon.size() < 3) {
            (*output)["error"] = "Polygon must have at least 3 points";
            return 3;
        }

        auto diagonals = TriangulateMonotonePolygon(polygon);

        nlohmann::json diagonals_json = nlohmann::json::array();
        for (const auto& diagonal : diagonals) {
            if (!IsPolygonEdge(polygon, diagonal.first, diagonal.second)) {
                diagonals_json.push_back({
                    {"from", diagonal.first},
                    {"to", diagonal.second}
                });
            }
        }

        (*output)["diagonals"] = diagonals_json;
        (*output)["diagonals_count"] = diagonals_json.size();
        (*output)["vertices_count"] = polygon.size();

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
 *   "polygon" : [
 *      {"x": 0.0, "y": 4.0},
 *      {"x": 2.0, "y": 3.0},
 *      {"x": 1.0, "y": 1.0},
 *      {"x": 4.0, "y": 0.0},
 *      {"x": 0.0, "y": 0.0}
 *   ]
 * }
 *
 * Output JSON structure:
 * {
 *   "diagonals" : [
 *     {"from": 0, "to": 3},
 *     {"from": 3, "to": 1}
 *   ],
 *   "diagonals_count" : 2,
 *   "vertices_count" : 5
 * }
 */
