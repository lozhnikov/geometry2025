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
 * @brief A method for implementing the algorithm for constructing a convex hull by the Graham method.
 * 
 * @param input input data in JSON format
 * @param output pointer to JSON output
 * @return return code: 0 - success, otherwise - error
 */

  int AlgGrahamMethod(const nlohmann::json& input, nlohmann::json* output);
  int GrahamScanMethod(const nlohmann::json& input, nlohmann::json* output);
  int AnglePointInPolygonMethod(const nlohmann::json& input, \
    nlohmann::json* output);

/* Конец вставки. */

}  // namespace geometry

#endif  // METHODS_METHODS_HPP_
