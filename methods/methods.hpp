/**
 * @file methods/methods.hpp
 * @author Nika Adzhindzhal
 *
 * Объявление функции для алгоритма построения звездчатого многоугольника.
 */

#ifndef METHODS_METHODS_HPP_
#define METHODS_METHODS_HPP_

#include <nlohmann/json.hpp>

namespace geometry {

	int My_PoligMethod(const nlohmann::json& input, nlohmann::json* output);


} // namespace geometry

#endif // METHODS_METHODS_HPP_
