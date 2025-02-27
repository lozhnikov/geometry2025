/**
 * @file tests/insertion_sort_test.cpp
 * @author Mikhail Lozhnikov
 *
 * Реализация набора тестов для алгоритма сортировки вставками.
 */

#include <insertion_sort.hpp>
#include <httplib.h>
#include <string>
#include <limits>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"


static void SimpleTest(httplib::Client* cli);
static void RandomTest(httplib::Client* cli);

template<typename T>
static void RandomIntegerHelperTest(httplib::Client* cli, std::string type);

template<typename T>
static void RandomFloatingPointHelperTest(httplib::Client* cli,
                                          std::string type);

void TestInsertionSort(httplib::Client* cli) {
  TestSuite suite("TestInsertionSort");

  RUN_TEST_REMOTE(suite, cli, SimpleTest);
  RUN_TEST_REMOTE(suite, cli, RandomTest);
}

/**
 * @brief Простейший статический тест.
 *
 * @param cli Указатель на HTTP клиент.
 */
static void SimpleTest(httplib::Client* cli) {
  {
    /*
     Библиотека nlohmann json позволяет преобразовать
     строку в объект nlohmann::json не только при помощи
     функции nlohmann::json::parse(), но и при помощи
     специального литерала _json. Если его поставить после строки
     в кавычках, то она конвертируется в json объект.

     R"(
     )" Так записывается строка, содержащая символы перевода строки
     в C++. Всё, что между скобками это символы строки. Перводы строк
     можно ставить просто как перевод строки в текстовом редактора
     (а не через \n).
    */
    nlohmann::json input = R"(
  {
    "id": 1,
    "type": "int",
    "size": 5,
    "data": [ 3, 2, 4, 5, 1 ]
  }
)"_json;

    /* Делаем POST запрос по адресу нашего метода на сервере.
    Метод dump() используется для преобразования JSON обратно в строку.
    (Можно было сразу строку передать). При передаче JSON данных
    необходимо поставить тип MIME "application/json".
    */
    httplib::Result res = cli->Post("/InsertionSort", input.dump(),
        "application/json");

    /* Используем метод parse() для преобразования строки ответа сервера
    (res->body) в объект JSON. */
    nlohmann::json output = nlohmann::json::parse(res->body);

    /* Проверка результатов сортировки. */
    REQUIRE_EQUAL(5, output["size"]);
    REQUIRE_EQUAL(1, output["id"]);
    REQUIRE_EQUAL("int", output["type"]);

    REQUIRE_EQUAL(output["data"][0], 1);
    REQUIRE_EQUAL(output["data"][1], 2);
    REQUIRE_EQUAL(output["data"][2], 3);
    REQUIRE_EQUAL(output["data"][3], 4);
    REQUIRE_EQUAL(output["data"][4], 5);
  }
}

/**
 * @brief Простейший случайный тест.
 *
 * @param cli Указатель на HTTP клиент.
 */
static void RandomTest(httplib::Client* cli) {
  RandomIntegerHelperTest<int>(cli, "int");

  RandomFloatingPointHelperTest<float>(cli, "float");
  RandomFloatingPointHelperTest<double>(cli, "double");
  RandomFloatingPointHelperTest<long double>(cli, "long double");
}

/**
 * @brief Простейший случайный тест для целых чисел.
 *
 * @tparam T Тип данных сортируемых элементов.
 *
 * @param cli Указатель на HTTP клиент.
 * @param type Строковое представление типа данных сортируемых элементов.
 *
 * Функция используется для сокращения кода, необходимого для поддержки
 * различных типов данных.
 */
template<typename T>
static void RandomIntegerHelperTest(httplib::Client* cli, std::string type) {
  // Число попыток.
  const int numTries = 100;
  // Используется для инициализации генератора случайных чисел.
  std::random_device rd;
  // Генератор случайных чисел.
  std::mt19937 gen(rd());
  // Распределение для количества элементов массива.
  std::uniform_int_distribution<size_t> arraySize(10, 50);
  // Распределение для элементов массива.
  std::uniform_int_distribution<T> elem(-10'000, 10'000);

  for (int it = 0; it < numTries; it++) {
    // Получаем случайный размер массива, используя функцию распределения.
    size_t size = arraySize(gen);

    nlohmann::json input;

    input["id"] = it;
    input["type"] = type;
    input["size"] = size;

    std::vector<T> data(size);

    for (size_t i = 0; i < size; i++) {
      // Получаем случайный элемент массива, используя функцию распределения.
      data[i] = elem(gen);

      // Записываем элемент в JSON.
      input["data"][i] = data[i];
    }

    /* Отправляем данные на сервер POST запросом. */
    httplib::Result res = cli->Post("/InsertionSort", input.dump(),
        "application/json");

    /* Используем метод parse() для преобразования строки ответа сервера
    (res->body) в объект JSON. */
    nlohmann::json output = nlohmann::json::parse(res->body);

    /* Проверка результатов сортировки. */

    std::sort(data.begin(), data.end());

    REQUIRE_EQUAL(size, output["size"]);
    REQUIRE_EQUAL(it, output["id"]);
    REQUIRE_EQUAL(type, output["type"]);

    for (size_t i = 0; i < size; i++) {
      REQUIRE_EQUAL(data[i], output["data"][i]);
    }
  }
}

/**
 * @brief Простейший случайный тест для чисел с плавающей точкой.
 *
 * @tparam T Тип данных сортируемых элементов.
 *
 * @param cli Указатель на HTTP клиент.
 * @param type Строковое представление типа данных сортируемых элементов.
 *
 * Функция используется для сокращения кода, необходимого для поддержки
 * различных типов данных.
 */
template<typename T>
static void RandomFloatingPointHelperTest(httplib::Client* cli,
                                          std::string type) {
  // Число попыток.
  const int numTries = 100;
  // Относительная точность сравнения.
  const T eps = std::numeric_limits<T>::epsilon() * T(1e4);

  // Используется для инициализации генератора случайных чисел.
  std::random_device rd;
  // Генератор случайных чисел.
  std::mt19937 gen(rd());
  // Распределение для количества элементов массива.
  std::uniform_int_distribution<size_t> arraySize(10, 50);
  // Распределение для элементов массива.
  std::uniform_real_distribution<T> elem(T(-10'000), T(10'000));

  for (int it = 0; it < numTries; it++) {
    // Получаем случайный размер массива, используя функцию распределения.
    size_t size = arraySize(gen);

    nlohmann::json input;

    input["id"] = it;
    input["type"] = type;
    input["size"] = size;

    std::vector<T> data(size);

    for (size_t i = 0; i < size; i++) {
      // Получаем случайный элемент массива, используя функцию распределения.
      data[i] = elem(gen);

      // Записываем элемент в JSON.
      input["data"][i] = data[i];
    }

    /* Отправляем данные на сервер POST запросом. */
    httplib::Result res = cli->Post("/InsertionSort", input.dump(),
        "application/json");

    /* Используем метод parse() для преобразования строки ответа сервера
    (res->body) в объект JSON. */
    nlohmann::json output = nlohmann::json::parse(res->body);

    /* Проверка результатов сортировки. */

    std::sort(data.begin(), data.end());

    REQUIRE_EQUAL(size, output["size"]);
    REQUIRE_EQUAL(it, output["id"]);
    REQUIRE_EQUAL(type, output["type"]);

    for (size_t i = 0; i < size; i++) {
      REQUIRE_CLOSE(data[i], T(output["data"][i]), eps);
    }
  }
}

