/**
 * @file methods/cyrus_bek_method.cpp
 *
 * @brief Cyrus-Bek line clipping algorithm implementation.
 *
 */

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "../include/cyrus_bek.hpp"

namespace geometry {

int CyrusBekMethod(const nlohmann::json& input, nlohmann::json* output) {
    try {
        // Validate input
        if (!input.contains("segment") || !input["segment"].is_object() ||
            !input.contains("polygon") || !input["polygon"].is_array()) {
            (*output)["error"] = "Input must contain 'segment' object and 'polygon' array";
            return 1;
        }

        // Parse segment
        auto& seg_json = input["segment"];
        if (!seg_json.contains("start") || !seg_json["start"].is_object() ||
            !seg_json.contains("end") || !seg_json["end"].is_object()) {
            (*output)["error"] = "Segment must contain 'start' and 'end' points";
            return 2;
        }

        Point<double> start(seg_json["start"]["x"].get<double>(),
                           seg_json["start"]["y"].get<double>());
        Point<double> end(seg_json["end"]["x"].get<double>(),
                         seg_json["end"]["y"].get<double>());
        Edge<double> segment(start, end);

        // Parse polygon
        std::vector<Point<double>> polygon_points;
        for (const auto& point_json : input["polygon"]) {
            polygon_points.emplace_back(
                point_json["x"].get<double>(),
                point_json["y"].get<double>());
        }
        Polygon<double> polygon(polygon_points);

        // Perform clipping
        Edge<double> result;
        bool visible = ClipLineSegment(segment, polygon, result);

        // Prepare output
        (*output)["visible"] = visible;
        if (visible) {
            (*output)["clipped_segment"] = {
                {"start", {{"x", result.Org().X()}, {"y", result.Org().Y()}}},
                {"end", {{"x", result.Dest().X()}, {"y", result.Dest().Y()}}}
            };
        }

        return 0;
    } catch (const std::exception& e) {
        (*output)["error"] = std::string("Exception: ") + e.what();
        return -1;
    }
}

} // namespace geometry

/**
 * Input JSON structure:
 * {
 *   "segment": {
 *     "start": {"x": 0.0, "y": 0.0},
 *     "end": {"x": 2.0, "y": 2.0}
 *   },
 *   "polygon": [
 *     {"x": 1.0, "y": 0.0},
 *     {"x": 2.0, "y": 1.0},
 *     {"x": 1.0, "y": 2.0},
 *     {"x": 0.0, "y": 1.0}
 *   ]
 * }
 *
 * Output JSON structure:
 * {
 *   "visible": true,
 *   "clipped_segment": {
 *     "start": {"x": 0.5, "y": 0.5},
 *     "end": {"x": 1.5, "y": 1.5}
 *   }
 * }
 */
