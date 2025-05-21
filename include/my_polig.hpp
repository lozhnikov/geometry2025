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

    /**
     * @brief Структура для представления точки с координатами x и y.
     * @tparam T Тип координат точки (по умолчанию double).
     */
    template<typename T = double>
    struct Point {
        T x, y;
        Point(T x = T(), T y = T()) : x(x), y(y) {}
    };

    /**
     * @brief Генерация точек на окружности.
     * @tparam T Тип координат точек.
     * @param n Количество вершин.
     * @param radius Радиус окружности.
     * @return Вектор точек типа Point<T>.
     */
    template<typename T>
    std::vector<Point<T>> GeneratePoints(int n, T radius) {
        std::vector<Point<T>> points;
        points.reserve(n);

        for (int i = 0; i < n; ++i) {
            T angle = static_cast<T>(2 * M_PI * i / n);
            points.emplace_back(
                radius * std::cos(angle),
                radius * std::sin(angle)
            );
        }
        return points;
    }

    /**
     * @brief Построение ребер звездчатого многоугольника.
     * @tparam IndexType Тип индексов вершин (по умолчанию int).
     * @param n Количество вершин.
     * @param k Шаг соединения.
     * @return Вектор пар индексов вершин.
     * @throw std::invalid_argument При невалидных параметрах.
     */
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

    /**
     * @brief Основной алгоритм построения звездчатого многоугольника.
     * @tparam T Тип координат точек.
     * @tparam IndexType Тип индексов вершин.
     * @param n Количество вершин.
     * @param k Шаг соединения.
     * @param radius Радиус окружности.
     * @return Пара: вектор точек и вектор ребер.
     */
    template<typename T = double, typename IndexType = int>
    std::pair<std::vector<Point<T>>, std::vector<std::pair<IndexType, IndexType>>>
        BuildStarPolygon(int n, int k, T radius = T(100.0)) {
        auto points = GeneratePoints<T>(n, radius);
        auto edges = BuildStarEdges<IndexType>(n, k);
        return { std::move(points), std::move(edges) };
    }

} // namespace geometry

#endif // INCLUDE_MY_POLIG_HPP_