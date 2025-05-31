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
 * @brief Серверная часть метода слияния двух выпуклых оболочек.
 *
 * @param input JSON с двумя оболочками: "left" и "right" — массивы точек [[x, y], ...]
 * @param output JSON с результатом — объединённая выпуклая оболочка как массив точек
 * @return 0 в случае упеха
 */
int GrahamScanMethod(const nlohmann::json& input, nlohmann::json* output);
int ShellMergeMethod(const nlohmann::json& input, nlohmann::json* output);

/* Конец вставки. */

}  // namespace geometry

#endif  // METHODS_METHODS_HPP_
