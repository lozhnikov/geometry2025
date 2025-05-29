/**
 * @file methods/methods.hpp
 * @author Mikhail Lozhnikov
 *
 * Объявления функций для серверной части алгоритмов. Эти функции должны
 * считать JSON, который прислал клиент, выполнить алгоритм и отправить клиенту
 * JSON с результатом работы алгоритма.
 */

#ifndef METHODS_METHODS_HPP_
#define METHODS_METHODS_HPP_

#include <nlohmann/json.hpp>

namespace geometry {

    /* Сюда нужно вставить объявление серверной части алгоритма. */

    /**
     * @brief Метод построения звездчатого многоугольника.
     *
     * @param input Входные данные в формате JSON.
     * @param output Выходные данные в формате JSON.
     * @return Функция возвращает 0 в случае успеха и отрицательное число
     * если входные данные заданы некорректно.
     *
     * Функция запускает алгоритм построения звездчатого многоугольника,
     * используя входные данные в JSON формате.
     */
    int My_PoligMethod(const nlohmann::json& input, nlohmann::json* output);
    /* Конец вставки. */


}  // namespace geometry

#endif  // METHODS_METHODS_HPP_
