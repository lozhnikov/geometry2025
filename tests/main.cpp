/**
 * @file tests/main.cpp
 * @author Mikhail Lozhnikov
 *
 * Файл с функией main() для клиентской части программы (набора тестов).
 */

#include <httplib.h>
#include <cstdio>
#include <string>
#include "test.hpp"
#include "test_core.hpp"

int main(int argc, char* argv[]) {
  // Порт по-умолчанию.
  int port = 8080;
  // Адрес сервера по-умолчанию.
  std::string host = "127.0.0.1";

  // Обычные тесты на сборку для базовых шаблонов.
  // Сюда ничего добавлять не нужно.
  TestPoint();
  TestEdge();
  TestPolygon();
  TestOrientation();

  if (argc >= 2) {
    // Меняем хост, если предоставлен соответствующий аргумент командной строки.
    host = std::string(argv[1]);
  }

  if (argc >= 3) {
    // Меняем порт, если предоставлен соответствующий аргумент командной строки.
    if (std::sscanf(argv[2], "%d", &port) != 1)
      return -1;
  }

  // Адрес подключения в формате хост:порт.
  std::string url = host + ":" + std::to_string(port);

  httplib::Client cli(url.c_str());

  /* Сюда нужно вставить вызов набора тестов для алгоритма. */
  
  // Локальные тесты алгоритма
  TestConvexPolygonIntersection();

  // Тесты через HTTP запросы
  TEST("HTTP: ConvexPolygonIntersection endpoint", {
    nlohmann::json input = {
      {"polygon1", {{{"x", 0}, {"y", 0}}, {{"x", 2}, {"y", 0}}, 
                   {{"x", 2}, {"y", 2}}, {{"x", 0}, {"y", 2}}}},
      {"polygon2", {{{"x", 1}, {"y", 1}}, {{"x", 3}, {"y", 1}}, 
                   {{"x", 3}, {"y", 3}}, {{"x", 1}, {"y", 3}}}}
    };

    auto res = cli.Post("/ConvexPolygonIntersection", 
                       input.dump(), 
                       "application/json");

    ASSERT(res->status == 200);
    
    auto output = nlohmann::json::parse(res->body);
    ASSERT(output.contains("result"));
    ASSERT(output["result"].size() == 4);
  });

  /* Конец вставки. */

  // Отправляем GET запрос для остановки сервера.
  httplib::Result res = cli.Get("/stop");

  if (res->status != 200)
    return -1;

  return TestSuite::Status();
}
