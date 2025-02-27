/**
 * @file tests/test.hpp
 * @author Mikhail Lozhnikov
 *
 * Заголовочный файл для объявлений основных тестовых функций.
 */

#ifndef TESTS_TEST_HPP_
#define TESTS_TEST_HPP_

#include <httplib.h>

/**
 * @brief Набор тестов для класса geometry::Point.
 */
void TestPoint();

/**
 * @brief Набор тестов для класса geometry::Edge.
 */
void TestEdge();

/**
 * @brief Набор тестов для класса geometry::Polygon.
 */
void TestPolygon();

/**
 * @brief Набор тестов для функции geometry::Orientation().
 */
void TestOrientation();

/* Сюда нужно добавить объявления тестовых функций. */

/**
 * @brief Набор тестов для алгоритма сортировки вставками.
 *
 * @param cli Указатель на HTTP клиент.
 */
void TestInsertionSort(httplib::Client* cli);


/* Конец вставки. */

#endif  // TESTS_TEST_HPP_
