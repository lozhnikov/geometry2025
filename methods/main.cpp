/**
 * @file methods/main.cpp
 * @author Nika Adzhindzhal
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
        if (std::sscanf(argv[1], "%d", &port) != 1)
            return -1;
    }
    std::cerr << "Listening on port " << port << "..." << std::endl;
    httplib::Server svr;

    svr.Get("/stop", [&](const httplib::Request&, httplib::Response&) {
        svr.stop();
        });

    /* Сюда нужно вставить обработчик post запроса для алгоритма. */

    svr.Post("/My_Polig",
        [&](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json input = nlohmann::json::parse(req.body);
        nlohmann::json output;

        if (geometry::My_PoligMethod(input, &output) < 0) {
            res.status = 400;
        }

        res.set_content(output.dump(), "application/json");
        });

    svr.listen("0.0.0.0", port);
    return 0;
}
