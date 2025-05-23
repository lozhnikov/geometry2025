/**
 * @file methods/main.cpp
 * @author Mikhail Lozhnikov
 *
 * Файл с функией main() для серверной части программы.
 */

#include <httplib.h>
#include <iostream>
#include <cstdio>
#include <nlohmann/json.hpp>
#include "methods.hpp"



int main(int argc, char* argv[]) {
  // Порт по-умолчанию.
  int port = 8080;

  if (argc >= 2) {
    // Меняем порт по умолчанию, если предоставлен соответствующий
    // аргумент командной строки.
    if (std::sscanf(argv[1], "%d", &port) != 1)
      return -1;
  }

  std::cerr << "Listening on port " << port << "..." << std::endl;

  httplib::Server svr;

  // Обработчик для GET запроса по адресу /stop. Этот обработчик
  // останавливает сервер.
  svr.Get("/stop", [&](const httplib::Request&, httplib::Response&) {
    svr.stop();
  });

  /* Сюда нужно вставить обработчик post запроса для алгоритма. */
 // Обработчик POST запроса для алгоритма пересечения многоугольников
  svr.Post("/intersect", [](const httplib::Request& req, httplib::Response& res) {
    try {

      auto json_data = json::parse(req.body);
      

      std::vector<Point> polygon1;
      for (auto& point : json_data["polygon1"]) {
        polygon1.push_back({point["x"], point["y"]});
      }
      

      std::vector<Point> polygon2;
      for (auto& point : json_data["polygon2"]) {
        polygon2.push_back({point["x"], point["y"]});
      }
      

      std::vector<Point> result = Process(polygon1, polygon2);
      
   
      json response;
      for (auto& point : result) {
        response["result"].push_back({{"x", point.x}, {"y", point.y}});
      }
      
      res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
      res.status = 400;
      res.set_content(json({{"error", e.what()}}).dump(), "application/json");
    }
  });


  /* Конец вставки. */

  // Эта функция запускает сервер на указанном порту. Программа не завершится
  // до тех пор, пока сервер не будет остановлен.
  svr.listen("0.0.0.0", port);

  return 0;
}
