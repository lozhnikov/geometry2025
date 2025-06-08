// Файл содержит реализацию тестов для алгоритма многоугольика Вороного


#include "../include/httplib.h"
#include <string>
#include <vector>
#include "../include/json.hpp"
#include "../include/test_core.hpp"
#include "../include/test.hpp"


void TestVoronoiPolygon(httplib::Client* cli) {	// запускаем только статический тест (без случайного)
	TestSuite suite("TestVoronoiPolygon");

	RUN_TEST_REMOTE(suite, cli, SimpleTest);
}


// простейший статический тест
static void SimpleTest(httplib::Client* cli) {
    {
       
        nlohmann::json input = R"(
  {
    "point": [200, 200],
	"points": [
		[100, 200],
		[200, 300],
		[300, 200],
		[200, 100]
	]
  }
)"_json;

        /* Делаем POST запрос по адресу нашего метода на сервере.
        Метод dump() используется для преобразования JSON обратно в строку.
        (Можно было сразу строку передать). При передаче JSON данных
        необходимо поставить тип MIME "application/json".
        */
        httplib::Result res = cli->Post("/VoronoiPolygon", input.dump(),
            "application/json");

        /* Используем метод parse() для преобразования строки ответа сервера
        (res->body) в объект JSON. */
        nlohmann::json output = nlohmann::json::parse(res->body);

        /* Проверка результатов. */
        REQUIRE_EQUAL(150, output["points"][0][0]);
        REQUIRE_EQUAL(250, output["points"][0][1]);

        REQUIRE_EQUAL(150, output["points"][1][0]);
        REQUIRE_EQUAL(150, output["points"][1][1]);

        REQUIRE_EQUAL(250, output["points"][2][0]);
        REQUIRE_EQUAL(150, output["points"][2][1]);

        REQUIRE_EQUAL(250, output["points"][3][0]);
        REQUIRE_EQUAL(250, output["points"][3][1]);

    }
}


