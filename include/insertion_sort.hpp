/**
 * @file include/insertion_sort.hpp
 * @author Mikhail Lozhnikov
 *
 * Реализация алгоритма сортировки вставками.
 */

#ifndef INCLUDE_INSERTION_SORT_HPP_
#define INCLUDE_INSERTION_SORT_HPP_

#include <cstddef>

namespace geometry {

/**
 * @brief Алгоритм сортировки вставками.
 *
 * @tparam T Тип данных сортируемых элементов.
 * @tparam Comparator Тип компаратора. Это может быть указатель
 * на функцию, класс с публичной операцией функционального вызова
 * или лямбда-выражение. Компаратор должен принимать два аргумента типа T,
 * возвращать 1, если второе число больше первого, -1 если
 * первое число больше второго и 0 если они равны.
 *
 * @param data Массив сортируемых элементов.
 * @param count Количество элементов в массиве.
 * @param comparator Алгоритм сравнения элементов.
 *
 * Функция сортирует массив при помощи сортировки слияния,
 * используя предоставленный метод сравнения элементов.
 */
template<typename T, typename Comparator>
void InsertionSort(T* data, size_t count, Comparator comparator) {
  for (size_t i = 1; i < count; i++) {
    T current = data[i];
    size_t j;

    for (j = i; j > 0; j--) {
      if (comparator(current, data[j - 1]) >= 0)
        break;

      data[j] = data[j - 1];
    }

    data[j] = current;
  }
}

}  // namespace geometry

#endif  // INCLUDE_INSERTION_SORT_HPP_
