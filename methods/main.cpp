/**
 * @file methods/main.cpp
 * @author Mikhail Lozhnikov
 *
 * @brief Main server application file.
 */

#include <nlohmann/json.hpp>
#include <httplib.h>
#include <iostream>
#include <cstdio>
#include <string>
#include "methods.hpp"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
  // Default port
  int port = 8080;
  if (argc >= 2) {
    // Change default port if provided as command line argument
    if (std::sscanf(argv[1], "%d", &port) != 1) {
      return -1;
    }
  }

  std::cerr << "Listening on port " << port << "..." << std::endl;

  httplib::Server svr;

  // GET handler for /stop endpoint to shutdown server
  svr.Get("/stop", [&](const httplib::Request&, httplib::Response&) {
    svr.stop();
  });

  // POST handler for convex intersection calculation
  svr.Post("/ConvexIntersection", 
      [&](const httplib::Request& req, httplib::Response& res) {
    try {
      auto input = json::parse(req.body);
      json output;
      int result = geometry::ConvexIntersectionMethod(input, &output);
      
      if (result != 0) {
        res.status = 400;
      }
      res.set_content(output.dump(), "application/json");
    } catch (const std::exception& e) {
      res.status = 400;
      res.set_content(json({{"error", e.what()}}).dump(), "application/json");
    }
  });

  svr.listen("0.0.0.0", port);
  return 0;
}
