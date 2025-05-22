/**
 * @file include/my_polig.hpp
 * @author Nika Adzhindzhal
 *
 * Реализация алгоритма построения звездчатого многоугольника.
 */

#ifndef INCLUDE_MY_POLIG_HPP_
#define INCLUDE_MY_POLIG_HPP_

#include <vector>
#include <cmath>
#include <utility>
#include <numeric>
#include <stdexcept>

namespace geometry {

    template<typename T = double>
    struct Point {
        T x, y;
        explicit Point(T x = T(), T y = T()) : x(x), y(y) {}
    };

    template<typename T>
    std::vector<Point<T>> GeneratePoints(int n, T radius) {
        std::vector<Point<T>> points;
        points.reserve(n);

        for (int i = 0; i < n; ++i) {
            T angle = static_cast<T>(2 * M_PI * i / n);
            points.emplace_back(
                radius * std::cos(angle),
                radius * std::sin(angle));
        }
        return points;
    }

    template<typename IndexType = int>
    std::vector<std::pair<IndexType, IndexType>> BuildStarEdges(int n, int k) {
        if (n < 3 || k <= 0 || k >= n || std::gcd(n, k) != 1) {
            throw std::invalid_argument("Invalid n/k parameters");
        }

        std::vector<std::pair<IndexType, IndexType>> edges;
        IndexType current = 0;
        const IndexType start = current;

        do {
            IndexType next = static_cast<IndexType>((current + k) % n);
            edges.emplace_back(current, next);
            current = next;
        } while (current != start);

        return edges;
    }

    template<typename T = double, typename IndexType = int>
    std::pair<
        std::vector<Point<T>>,
        std::vector<std::pair<IndexType, IndexType>>
    > BuildStarPolygon(int n, int k, T radius = T(100.0)) { 
        auto points = GeneratePoints<T>(n, radius);
        auto edges = BuildStarEdges<IndexType>(n, k);
        return { std::move(points), std::move(edges) };
    }

}  // namespace geometry

#endif  // INCLUDE_MY_POLIG_HPP_
