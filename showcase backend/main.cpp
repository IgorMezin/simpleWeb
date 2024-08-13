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
        
        if (req.method == "GET"_method) {
            std::string content = read_file("register.html");
            res.set_header("Content-Type", "text/html; charset=UTF-8");
            res.write(content);
            res.end();
        }
        else if (req.method == "POST"_method) {
            crow::multipart::message multipart_message(req);

            auto fio_part = multipart_message.get_part_by_name("fio");
            auto specialty_part = multipart_message.get_part_by_name("specialty");
            auto subject_scores_part = multipart_message.get_part_by_name("subject_scores");
            int study_type = multipart_message.get_part_by_name("study_type");
            studyType st_type = convertIntToStudyType(study_type);
            auto pdf_file_part = multipart_message.get_part_by_name("pdf_file");
            if (!pdf_file_part.body.empty()) {
                std::string pdf_filename = pdf_file_part.get_header_object("Content-Disposition").params.at("filename");
                save_uploaded_file(pdf_filename, pdf_file_part.body);
            }

            if(add_user_to_db(hDbc, fio_part.body, specialty_part.body, subject_scores_part.body, st_type)){ }
            
            //save_user_data(fio_part.body, specialty_part.body, subject_scores_part.body, study_type_part.body);
            res.code = 302; // HTTP статус для перенаправления
            res.set_header("Location", "/users");
            res.end();
        }
        });


    CROW_ROUTE(app, "/users").methods("GET"_method)
        ([&](const crow::request& req, crow::response& res) {
        if (req.method == "GET"_method) {
            std::vector<User> users = get_all_users_from_db(hDbc);
            std::string template_content = read_file("users.html");
            std::string user_rows;

            for (const auto& user : users) {
                user_rows += "<tr>"
                    "<td>" + user.fio + "</td>"
                    "<td>" + user.specialty + "</td>"
                    "<td>" + user.subjectScores + "</td>"
                    "<td>" + user.study_type + "</td>"
                    "</tr>";
            }

            // Вставка строк пользователей в шаблон
            size_t pos = template_content.find("<!-- USER_ROWS_MARKER -->");
            if (pos != std::string::npos) {
                template_content.replace(pos, std::string("<!-- USER_ROWS_MARKER -->").length(), user_rows);
            }

            res.set_header("Content-Type", "text/html; charset=UTF-8");
            res.write(template_content);
            res.end();
        }
        });


    app.port(18080).multithreaded().run();

    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, SQL_NULL_HENV);

    return 0;
}
