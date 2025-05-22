/**
 * @file methods/methods.hpp
 * @author Mikhail Lozhnikov
 * 
 * Объявления функий для серверной части алгоритмов построения звездчатого многоугольника.. Эти функции должны
 * считать JSON, который прислал клиент, выполнить алгоритм и отправить клиенту
 * JSON с результатом работы алгоритма.
 */

#ifndef METHODS_METHODS_HPP_
#define METHODS_METHODS_HPP_

namespace geometry {

	int My_PoligMethod(const nlohmann::json& input, nlohmann::json* output);

} // namespace geometry

#endif// METHODS_METHODS_HPP_
