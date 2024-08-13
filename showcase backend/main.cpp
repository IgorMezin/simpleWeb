#include "crow.h"
#include "crow/middlewares/cookie_parser.h"
#include "utils.h"
#include "databaseManipulate.h"
#include <boost/asio.hpp>



int main()
{

    // определяем app и middleware, в данном случае определим куки
    crow::App<crow::CookieParser> app;
    

    // Определение маршрута для страницы регистрации
    CROW_ROUTE(app, "/register").methods("GET"_method, "POST"_method)
        ([&](const crow::request& req, crow::response& res) {
        
        });
    app.port(18080).multithreaded().run();

    return 0;
}
