/**
 * @file methods/main.cpp
 * @author Mikhail Lozhnikov
 *
 * Файл с функией main() для серверной части программы.
 */

#include <nlohmann/json.hpp>
#include <httplib.h>
#include <iostream>
#include <cstdio>
#include <string>
#include "methods.hpp"


 using json = nlohmann::json;

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
  std::string host = "127.0.0.1";

  /* Сюда нужно вставить обработчик post запроса для алгоритма. */

  svr.Post("/ConvexIntersection", [&](const httplib::Request& req, httplib::Response& res) {
  try {
    auto input = json::parse(req.body);
    json output;
    int result = geometry::ConvexIntersectionMethod(input, &output);
    if (result != 0)
        res.status = 400;
    res.set_content(output.dump(), "application/json");
  } catch (const std::exception& e) {
    res.status = 400;
    res.set_content(json({{"error", e.what()}}).dump(), "application/json");
  }
});
svr.listen("0.0.0.0", port);
return 0;
}

  /* Конец вставки. */
