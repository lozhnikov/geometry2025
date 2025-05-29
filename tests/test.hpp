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

/* Объявления тестовых функций */

/**
 * @brief testing Graham scan
 * 
 * @param cli pointer to HTTP client
 */

/**
 * @brief Тестирование функции пересечения выпуклых полигонов
 */
void TestConvexIntersection(httplib::Client* cli);



#endif  // TESTS_TEST_HPP_
