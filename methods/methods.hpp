/**
 * @file methods/methods.hpp
 * @author Mikhail Lozhnikov
 *
 * Объявления функий для серверной части алгоритмов. Эти функции должны
 * считать JSON, который прислал клиент, выполнить алгоритм и отправить клиенту
 * JSON с результатом работы алгоритма.
 */

#ifndef METHODS_METHODS_HPP_
#define METHODS_METHODS_HPP_

namespace geometry {

/* Сюда нужно вставить объявление серверной части алгоритма. */

/** 
 * @brief Method for Sutherland-Hodgman algorithm realization.
 * 
 * @param input input data in JSON format
 * @param output pointer to JSON output
 * @return return code: 0 - success, otherwise - error
 */

  int SutherlandHodgmanMethod(const nlohmann::json& input, nlohmann::json* output);

/* Конец вставки. */

}  // namespace geometry

#endif  // METHODS_METHODS_HPP_
