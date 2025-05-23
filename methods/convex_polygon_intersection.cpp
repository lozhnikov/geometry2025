// methods/convex_polygon_intersection_method.cpp
#include "../include/convex_polygon_intersection.hpp"
#include "methods.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {

template <typename T>
bool isInside(const Point<T>& p, const Point<T>& a, const Point<T>& b) {
    return ((b - a).cross(p - a)) >= 0;
}

template <typename T>
Point<T> intersection(const Point<T>& a1, const Point<T>& a2, 
                     const Point<T>& b1, const Point<T>& b2) {
    Point<T> da = a2 - a1;
    Point<T> db = b2 - b1;
    Point<T> dp = a1 - b1;
    auto dap_cross_db = da.cross(db);
    T t = db.cross(dp) / dap_cross_db;
    return {a1.x + t * da.x, a1.y + t * da.y};
}

template <typename T>
bool isSamePoint(const Point<T>& a, const Point<T>& b, T eps = 1e-9) {
    return std::fabs(a.x - b.x) < eps && std::fabs(a.y - b.y) < eps;
}

} // namespace

template <typename T>
std::vector<Point<T>> convexPolygonIntersection(
    const std::vector<Point<T>>& polygon1,
    const std::vector<Point<T>>& polygon2) {
    
    std::vector<Point<T>> output = polygon1;
    
    for (size_t i = 0; i < polygon2.size(); ++i) {
        Point<T> A = polygon2[i];
        Point<T> B = polygon2[(i + 1) % polygon2.size()];
        
        std::vector<Point<T>> input = output;
        output.clear();
        
        for (size_t j = 0; j < input.size(); ++j) {
            Point<T> P = input[j];
            Point<T> Q = input[(j + 1) % input.size()];
            
            bool P_inside = isInside(P, A, B);
            bool Q_inside = isInside(Q, A, B);
            
            if (P_inside && Q_inside) {
                output.push_back(Q);
            } else if (P_inside && !Q_inside) {
                output.push_back(intersection(P, Q, A, B));
            } else if (!P_inside && Q_inside) {
                output.push_back(intersection(P, Q, A, B));
                output.push_back(Q);
            }
        }
    }
    
    std::vector<Point<T>> unique;
    for (const auto& p : output) {
        bool exists = false;
        for (const auto& q : unique) {
            if (isSamePoint(p, q)) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            unique.push_back(p);
        }
    }
    
    return unique;
}

int ConvexPolygonIntersectionMethod(const json& input, json* output) {
    try {
   
        if (!input.contains("polygon1") || !input.contains("polygon2")) {
            return -1;
        }

        std::vector<Point<double>> polygon1, polygon2;
        
        for (auto& point : input["polygon1"]) {
            if (!point.contains("x") || !point.contains("y")) {
                return -1;
            }
            polygon1.push_back({point["x"], point["y"]});
        }
        
        for (auto& point : input["polygon2"]) {
            if (!point.contains("x") || !point.contains("y")) {
                return -1;
            }
            polygon2.push_back({point["x"], point["y"]});
        }
        
        if (polygon1.size() < 3 || polygon2.size() < 3) {
            return -1;
        }
       
        auto result = convexPolygonIntersection(polygon1, polygon2);
        
        if (result.empty()) {
            (*output)["result"] = json::array();
        } else {
            for (auto& point : result) {
                (*output)["result"].push_back({{"x", point.x}, {"y", point.y}});
            }
        }
        
        return 0;
    } catch (...) {
        return -1;
    }
}