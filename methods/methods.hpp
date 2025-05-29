/**
 * @file methods/methods.hpp
 * @author Mikhail Lozhnikov
 *
 * Объявления функций для серверной части алгоритмов. Эти функции должны
 * считать JSON, который прислал клиент, выполнить алгоритм и отправить
 * клиенту JSON с результатом работы алгоритма.
 */

#ifndef METHODS_METHODS_HPP_
#define METHODS_METHODS_HPP_

#include <nlohmann/json.hpp>

namespace geometry {

int ConvexIntersectionMethod(const nlohmann::json& input,
                             nlohmann::json* output);

}  // namespace geometry

#endif  // METHODS_METHODS_HPP_
