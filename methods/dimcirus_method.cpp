/**
 * @file methods/dimcirus_method.cpp
 * @author Dmitrii Chebanov
 *
 * @brief Cyrus-Beck line clipping algorithm implementation.
 */

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "../include/methods.hpp"
#include "../include/clip_algorithm.hpp"
#include "../include/common.hpp"
#include "../include/edge.hpp"
#include "../include/polygon.hpp"

/**
 * @brief Method for Cyrus-Beck line clipping algorithm implementation.
 *
 * @param input input data in JSON format
 * @param output pointer to JSON output
 * @return return code: 0 - success, otherwise - error
 */

namespace geometry {
    int DimcirusMethod(const nlohmann::json& input, nlohmann::json* output) {
        try {
            // Validate input structure
            if (!input.contains("segment") || !input["segment"].is_object()) {
                (*output)["error"] = "Input must contain 'segment' object";
                return 1;
            }
            if (!input.contains("polygon") || !input["polygon"].is_object()) {
                (*output)["error"] = "Input must contain 'polygon' object";
                return 1;
            }
            if (!input.contains("type") || !input["type"].is_string()) {
                (*output)["error"] = "Input must contain 'type' string field";
                return 1;
            }

            // Get type and call appropriate helper
            std::string type = input["type"].get<std::string>();
            if (type == "float") {
                return DimcirusMethodHelper<float>(input, output, type);
            } else if (type == "double") {
                return DimcirusMethodHelper<double>(input, output, type);
            } else {
                (*output)["error"] = "Invalid type, must be 'float' or 'double'";
                return 2;
            }
        } catch (const std::exception& e) {
            (*output)["error"] = std::string("Exception: ") + e.what();
            return -1;
        }
    }

    template<typename T>
    static int DimcirusMethodHelper(const nlohmann::json& input,
                                   nlohmann::json* output,
                                   const std::string& type) {
        try {
            (*output)["id"] = input.at("id");

            // Parse segment
            const auto& segment = input.at("segment");
            const auto& origin = segment.at("origin");
            const auto& destination = segment.at("destination");
            
            if (!origin.contains("x") || !origin.contains("y") ||
                !destination.contains("x") || !destination.contains("y")) {
                (*output)["error"] = "Segment points must have 'x' and 'y' fields";
                return 2;
            }

            T ox = origin.at("x").get<T>();
            T oy = origin.at("y").get<T>();
            T dx = destination.at("x").get<T>();
            T dy = destination.at("y").get<T>();
            
            Edge<T> edge(Point<T>(ox, oy), Point<T>(dx, dy));

            // Parse polygon
            const auto& polygon = input.at("polygon");
            const auto& vertices = polygon.at("vertices");
            
            if (!vertices.is_array() || vertices.size() < 3) {
                (*output)["error"] = "Polygon must have at least 3 vertices";
                return 3;
            }

            Polygon<T> poly;
            for (const auto& vertex : vertices) {
                if (!vertex.contains("x") || !vertex.contains("y")) {
                    (*output)["error"] = "Each vertex must have 'x' and 'y' fields";
                    return 4;
                }
                T x = vertex.at("x").get<T>();
                T y = vertex.at("y").get<T>();
                poly.Insert(Point<T>(x, y));
            }

            // Perform clipping
            Edge<T> result;
            bool success = clipLineSegment(edge, poly, result);

            // Prepare output
            (*output)["success"] = success;
            if (success) {
                (*output)["result"]["origin"]["x"] = result.Origin().X();
                (*output)["result"]["origin"]["y"] = result.Origin().Y();
                (*output)["result"]["destination"]["x"] = result.Destination().X();
                (*output)["result"]["destination"]["y"] = result.Destination().Y();
            }
            (*output)["type"] = type;

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
 *   "id": "unique_id",
 *   "type": "float",
 *   "segment": {
 *     "origin": {"x": 0.0, "y": 0.0},
 *     "destination": {"x": 1.0, "y": 1.0}
 *   },
 *   "polygon": {
 *     "vertices": [
 *       {"x": 0.0, "y": 0.5},
 *       {"x": 0.5, "y": 1.0},
 *       {"x": 1.0, "y": 0.5},
 *       {"x": 0.5, "y": 0.0}
 *     ]
 *   }
 * }
 *
 * Output JSON structure:
 * {
 *   "id": "unique_id",
 *   "success": true,
 *   "type": "float",
 *   "result": {
 *     "origin": {"x": 0.0, "y": 0.5},
 *     "destination": {"x": 0.5, "y": 0.0}
 *   }
 * }
 */
