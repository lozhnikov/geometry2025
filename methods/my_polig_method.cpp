/**
 * @file methods/my_polig_method.cpp
 * @author Nika Adzhindzhal
 *
 */

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <utility>
#include <nlohmann/json.hpp>
#include "methods.hpp"

namespace geometry {

    struct Point {
        double x, y;
    };

    int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }

    std::vector<Point> generate_points(int n, double radius) {
        std::vector<Point> points;
        for (int i = 0; i < n; ++i) {
            double angle = 2 * M_PI * i / n;
            points.push_back({ radius * cos(angle), radius * sin(angle) });
        }
        return points;
    }

    std::vector<std::pair<int, int>> build_star_edges(int n, int k) {
        std::vector<std::pair<int, int>> edges;
        int current = 0;
        int start = current;
        do {
            int next = (current + k) % n;
            edges.emplace_back(current, next);
            current = next;
        } while (current != start);
        return edges;
    }

    int My_PoligMethod(const nlohmann::json& input, nlohmann::json* output) {
        int n, k;
        double radius = 100.0;

        try {
            n = input.at("n");
            k = input.at("k");
            if (input.contains("radius")) {
                radius = input.at("radius");
            }
        }
        catch (const nlohmann::json::exception& e) {
            return -1;
        }

        if (n < 3 || k <= 0 || k >= n || gcd(n, k) != 1) {
            std::cout << "Incorrect input data!" << std::end;
            return -1;
        }

        auto points = generate_points(n, radius);
        auto edges = build_star_edges(n, k);

        (*output)["vertices"] = nlohmann::json::array();
        for (const auto& p : points) {
            nlohmann::json vertex;
            vertex["x"] = p.x;
            vertex["y"] = p.y;
            (*output)["vertices"].push_back(vertex);
        }

        (*output)["edges"] = nlohmann::json::array();
        for (const auto& e : edges) {
            nlohmann::json edge;
            edge["a"] = e.first;
            edge["b"] = e.second;
            (*output)["edges"].push_back(edge);
        }

        return 0;
    }

}  // namespace geometry
