/** 
 * @file methods/convex_intersection_method.cpp 
 * @author German Semenov
 * 
 * @brief convex intersection algorithm.
 */

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "../include/convex_intersection.hpp"

namespace geometry {

int ConvexIntersectionMethod(const nlohmann::json& input, nlohmann::json* output) {
  if (!input.contains("subject") || !input.contains("clip"))
    return 1;

  std::vector<Point<double>> subject, clip;

  for (const auto& p : input["subject"])
    subject.emplace_back(p["x"], p["y"]);

  for (const auto& p : input["clip"])
    clip.emplace_back(p["x"], p["y"]);

  auto result = ConvexIntersection(subject, clip);

  for (const auto& p : result)
    (*output)["intersection"].push_back({{"x", p.X()}, {"y", p.Y()}});

  (*output)["subject_size"] = subject.size();
  (*output)["clip_size"] = clip.size();
  (*output)["result_size"] = result.size();

  return 0;
}

}
