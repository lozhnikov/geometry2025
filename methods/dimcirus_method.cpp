/**
 * @file methods/dimcirus_method.cpp
 * @author Dmitrii Chebanov
 *
 */

#include <common.hpp>
#include <edge.hpp>
#include <polygon.hpp>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include "methods.hpp"

namespace geometry {

template<typename T>
static int DimcirusMethodHelper(const nlohmann::json& input,
                                nlohmann::json* output,
                                const std::string& type) {
  try {
    (*output)["id"] = input.at("id");

    const auto& segment = input.at("segment");
    const auto& origin = segment.at("origin");
    const auto& destination = segment.at("destination");
    
    T ox = origin.at("x");
    T oy = origin.at("y");
    T dx = destination.at("x");
    T dy = destination.at("y");
    
    Edge<T> edge(Point<T>(ox, oy), Point<T>(dx, dy));

    const auto& polygon = input.at("polygon");
    const auto& vertices = polygon.at("vertices");
    if (vertices.size() < 3)
      return -1;

    Polygon<T> poly;
    for (const auto& vertex : vertices) {
      T x = vertex.at("x");
      T y = vertex.at("y");
      poly.Add(Point<T>(x, y));
    }

    Edge<T> result;
    bool success = clipLineSegment(edge, poly, result);

    (*output)["success"] = success;
    if (success) {
      (*output)["result"]["origin"]["x"] = result.Origin().X();
      (*output)["result"]["origin"]["y"] = result.Origin().Y();
      (*output)["result"]["destination"]["x"] = result.Destination().X();
      (*output)["result"]["destination"]["y"] = result.Destination().Y();
    }
    (*output)["type"] = type;

    return 0;
  } catch (const nlohmann::json::exception& e) {
    return -1;
  } catch (...) {
    return -1;
  }
}

int DimcirusMethod(const nlohmann::json& input, nlohmann::json* output) {
  try {
    std::string type = input.at("type");
    if (type == "float")
      return DimcirusMethodHelper<float>(input, output, type);
    else if (type == "double")
      return DimcirusMethodHelper<double>(input, output, type);
    else
      return -1;
  } catch (const nlohmann::json::exception& e) {
    return -1;
  }
}

}
