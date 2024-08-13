#include "crow.h"
#include "crow/middlewares/cookie_parser.h"
#include "utils.h"
#include "databaseManipulate.h"
#include <boost/asio.hpp>



int main()
{

    // определяем app и middleware, в данном случае определим куки
    crow::App<crow::CookieParser> app;
    
    std::string connection_string = "DRIVER={PostgreSQL ODBC Driver(UNICODE)};SERVER=localhost;PORT=5432;DATABASE=spbgu;UID=postgres;PWD=1234;";
    // определяем соединение с базой
    SQLHDBC hDbc = connect_to_db(connection_string);
    if (hDbc == SQL_NULL_HDBC) {
        return 1;
    }

    // Определение маршрута для страницы регистрации
    CROW_ROUTE(app, "/register").methods("GET"_method, "POST"_method)
        ([&](const crow::request& req, crow::response& res) {
        
        });


    app.port(18080).multithreaded().run();

    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, SQL_NULL_HENV);

    return 0;
}
