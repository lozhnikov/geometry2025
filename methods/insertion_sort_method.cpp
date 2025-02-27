/**
 * @file methods/insertion_sort_method.cpp
 * @author Mikhail Lozhnikov
 *
 * Файл содержит функцию, которая вызывает алгоритм сортировки вставками.
 * Функция принимает и возвращает данные в JSON формате.
 */

#include <string>
#include <nlohmann/json.hpp>
#include "insertion_sort.hpp"

namespace geometry {

template<typename T>
static int InsertionSortMethodHelper(const nlohmann::json& input,
                                     nlohmann::json* output,
                                     std::string type);

int InsertionSortMethod(const nlohmann::json& input, nlohmann::json* output) {
  /*
  С классом nlohmann::json можно работать как со словарём.
  Метод at() в отличие оператора [] не меняет объект, поэтому
  этот метод можно использовать с константными объектами.
  */
  std::string type = input.at("type");

  /* Пока реализована только поддержка целых чисел и чисел типа double. */
  if (type == "int") {
    return InsertionSortMethodHelper<int>(input, output, type);
  } else if (type == "float") {
    return InsertionSortMethodHelper<float>(input, output, type);
  } else if (type == "double") {
    return InsertionSortMethodHelper<double>(input, output, type);
  } else if (type == "long double") {
    return InsertionSortMethodHelper<long double>(input, output, type);
  }

  return -1;
}

/**
 * @brief Метод сортировки вставками.
 *
 * @tparam T Тип данных сортируемых элементов.
 *
 * @param input Входные данные в формате JSON.
 * @param output Выходные данные в формате JSON.
 * @param type Строковое представление типа данных мортируемых элементов.
 * @return Функция возвращает 0 в случае успеха и отрицательное число
 * если входные данные заданы некорректно.
 *
 * Функция запускает алгоритм сортировки вставками, используя входные данные
 * в JSON формате. Результат также выдаётся в JSON формате. Функция
 * используется для сокращения кода, необходимого для поддержки различных типов данных.
 */
template<typename T>
static int InsertionSortMethodHelper(const nlohmann::json& input,
                                     nlohmann::json* output,
                                     std::string type) {
  (*output)["id"] = input.at("id");

  size_t size = input.at("size");

  T* data = new T[size];

  for (size_t i = 0; i < size; i++) {
    /* Для словарей используется индекс в виде строки,
    а для массивов просто целое число типа size_t. */
    data[i] = input.at("data").at(i);
  }

  /* Здесь вызывается сам алгоритм сортировки вставками. */
  InsertionSort(data, size, [](T left, T right) {
    if (left < right)
      return -1;
    else if (left > right)
      return 1;

    return 0;
  });

  /* Сохраняем в ответе результат работы алгоритма. */
  (*output)["size"] = size;
  (*output)["type"] = type;

  for (size_t i = 0; i < size; i++)
    (*output)["data"][i] = data[i];

  delete[] data;

  return 0;
}

}  // namespace geometry
