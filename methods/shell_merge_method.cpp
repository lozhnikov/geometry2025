/**
 * @file methods/shell_merge_method.cpp
 * @author Щербинин Виталий
 * @brief Реализация метода слияния выпуклых оболочек для серверной части.
 *
 */

#include <iostream>
#include "shell_merge.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using geometry::Polygon;
using geometry::Point;
using geometry::MergeHulls;

/**
* @param input Входной JSON с полями "left" и "right" — списками точек
* @param output Выходной JSON — список вершин объединённой оболочки
* @return 0 при успешном выполнении, -1 при ошибке
*/

namespace geometry {

int ShellMergeMethod(const json& input, json* output) {
    try {
        Polygon left, right;

        for (const auto& p : input.at("left")) {
            left.insert(Point{p[0].get<double>(), p[1].get<double>()});
        }
        for (const auto& p : input.at("right")) {
            right.insert(Point{p[0].get<double>(), p[1].get<double>()});
        }


        Polygon* merged = MergeHulls(&left, &right);

        if (merged->head) {
            Vertex* v = merged->head;
            do {
                output->push_back(json::array({v->pt.x, v->pt.y}));
                v = v->next;
            } while (v != merged->head);
        }

        delete merged;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in ShellMergeMethod: " << e.what() << std::endl;
        return -1;
    }
}

}  // namespace geometry
