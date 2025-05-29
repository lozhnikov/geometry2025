/**
 * @file tests/my_polig_test.cpp
 * @author Nika Adzhindzhal
 *
 * ���������� ������ ������ ��� ��������� ���������� ��������� ��������������.
 */

#include <point.hpp>
#include <polygon.hpp>
#include <httplib.h>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include "test_core.hpp"
#include "test.hpp"

namespace geometry {

    template<typename T>
    int PolarCmpForTest(const Point<T>& p, const Point<T>& q,
        const Point<T>& origin, T precision) {
        Point<T> vp = p - origin;
        Point<T> vq = q - origin;
        T angle_p = vp.PolarAngle(precision);
        T angle_q = vq.PolarAngle(precision);

        if (angle_p < angle_q) return -1;
        if (angle_p > angle_q) return 1;

        T len_p = vp.Length();
        T len_q = vq.Length();

        if (len_p < len_q) return -1;
        if (len_p > len_q) return 1;
        return 0;
    }

}  // namespace geometry

/* static void SimpleTest(httplib::Client* cli); */
static void RandomTest(httplib::Client* cli);

void TestMyPolig(httplib::Client* cli) {
    TestSuite suite("TestMyPolig");
    /*RUN_TEST_REMOTE(suite, cli, SimpleTest); */
    RUN_TEST_REMOTE(suite, cli, RandomTest);
}

/* static void SimpleTest(httplib::Client* cli) {
    nlohmann::json input = R"({
        "precision": 1e-5,
        "points": [
            {"x": 0.0, "y": 0.0},
            {"x": 0.0, "y": 1.0},
            {"x": 1.0, "y": 1.0},
            {"x": 1.0, "y": 0.0}
        ]
    })"_json;

    std::string request_body = input.dump();
    auto res = cli->Post("/MyPolig", request_body, "application/json");

    // �������� ���������� �������
    REQUIRE(res != nullptr);
    if (res->status != 200) {
        std::cerr << "===== REQUEST =====" << std::endl;
        std::cerr << "POST /MyPolig" << std::endl;
        std::cerr << "Body: " << request_body << std::endl;
        std::cerr << "===== RESPONSE =====" << std::endl;
        std::cerr << "Status: " << res->status << std::endl;
        std::cerr << "Body: " << res->body << std::endl;
        std::cerr << "====================" << std::endl;
    }
    REQUIRE_EQUAL(200, res->status);

    try {
        nlohmann::json output = nlohmann::json::parse(res->body);

        REQUIRE_EQUAL(4, output["vertices"].size());

        std::vector<geometry::Point<double>> expected;
        expected.push_back(geometry::Point<double>(0.0, 0.0));
        expected.push_back(geometry::Point<double>(0.0, 1.0));
        expected.push_back(geometry::Point<double>(1.0, 1.0));
        expected.push_back(geometry::Point<double>(1.0, 0.0));

        std::cout << output.dump() << std::endl;

        for (size_t i = 0; i < 4; i++) {
            REQUIRE_EQUAL(expected[i].X(), output["vertices"][i]["x"]);
            REQUIRE_EQUAL(expected[i].Y(), output["vertices"][i]["y"]);
        }
    }
    catch (const nlohmann::json::exception& e) {
        REQUIRE(false && ("JSON parse error: " + std::string(e.what()) +
            "\nResponse body: " + res->body).c_str());
    }
}
*/

static void RandomTest(httplib::Client* cli) {
    const int numTries = 5;
    const double precision = 1e-5;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> coord(-10.0, 10.0);
    std::uniform_int_distribution<size_t> sizeDist(3, 10);

    for (int it = 0; it < numTries; it++) {
        size_t size = sizeDist(gen);
        nlohmann::json input;
        input["precision"] = precision;
        input["points"] = nlohmann::json::array();

        double x, y;
        do {
            x = coord(gen);
            y = coord(gen);
        } while (std::abs(x) < 0.1 && std::abs(y) < 0.1);

        geometry::Point<double> origin(x, y);
        input["points"].push_back({ {"x", origin.X()}, {"y", origin.Y()} });

        std::vector<geometry::Point<double>> points;
        points.push_back(origin);

        for (size_t i = 1; i < size; i++) {
            double px, py;
            do {
                px = coord(gen);
                py = coord(gen);
            } while (std::abs(px - origin.X()) < 0.1 &&
                std::abs(py - origin.Y()) < 0.1);

            geometry::Point<double> p(px, py);
            input["points"].push_back({ {"x", p.X()}, {"y", p.Y()} });
            points.push_back(p);
        }

        std::string request_body = input.dump();
        auto res = cli->Post("/MyPolig", request_body, "application/json");

        REQUIRE(res != nullptr);
        if (res->status != 200) {
            std::cerr << "===== REQUEST (try " << it << ") =====" << std::endl;
            std::cerr << "POST /MyPolig" << std::endl;
            std::cerr << "Body: " << request_body << std::endl;
            std::cerr << "===== RESPONSE =====" << std::endl;
            std::cerr << "Status: " << res->status << std::endl;
            std::cerr << "Body: " << res->body << std::endl;
            std::cerr << "====================" << std::endl;
        }
        REQUIRE_EQUAL(200, res->status);

        try {
            nlohmann::json output = nlohmann::json::parse(res->body);
            REQUIRE_EQUAL(size, output["vertices"].size());

            std::vector<geometry::Point<double>> vertices;
            for (const auto& vertex : output["vertices"]) {
                vertices.emplace_back(
                    vertex["x"].get<double>(),
                    vertex["y"].get<double>() );
            }

            REQUIRE_EQUAL(origin.X(), vertices[0].X());
            REQUIRE_EQUAL(origin.Y(), vertices[0].Y());

            std::vector<geometry::Point<double>> polyPoints(
                vertices.begin() + 1, vertices.end());
            std::vector<geometry::Point<double>> inputPoints(points.begin() + 1,
                points.end());

            std::sort(inputPoints.begin(), inputPoints.end(),
                [&](const geometry::Point<double>& a,
                    const geometry::Point<double>& b) {
                    int cmp = geometry::PolarCmpForTest(a, b,
                        origin, precision);
                    if (cmp != 0) return cmp > 0;
                    return (a - origin).Length() < (b - origin).Length();
                });

            for (size_t i = 0; i < polyPoints.size(); i++) {
                REQUIRE_EQUAL(inputPoints[i].X(), polyPoints[i].X());
                REQUIRE_EQUAL(inputPoints[i].Y(), polyPoints[i].Y());
            }
        }
        catch (const nlohmann::json::exception& e) {
            REQUIRE(false && ("JSON parse error: " + std::string(e.what()) +
                "\nResponse body: " + res->body).c_str());
        }
    }
}