/**
 * @file methods/main.cpp
 * @author Mikhail Lozhnikov, Dmitrii Chebanov
 *
 * Файл с функией main() для серверной части программы.
 */

#include <httplib.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <nlohmann/json.hpp>
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

  server.Post("/CyrusBek",[](const httplib::Request& req,
                             httplib::Response& res) {
    nlohmann::json input, output;
    try {
      input = nlohmann::json::parse(req.body);
      } catch (...) {
        output["error"] = "Invalid JSON";
        res.set_content(output.dump(), "application/json");
        return;
        }
        
        int ret = CyrusBekMethod(input, &output);
        res.status = ret == 0 ? 200 : 400;
        res.set_content(output.dump(), "application/json");
  });

  svr.listen("0.0.0.0", port);

  return 0;
}
