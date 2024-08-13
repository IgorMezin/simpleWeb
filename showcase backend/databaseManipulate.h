#pragma once
#include "studyTypeHelp.h"
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <string>
#include <iostream>
#include <vector>


struct User {
    std::string fio;
    std::string specialty;
    std::string subjectScores;
    std::string study_type;
};

SQLHDBC connect_to_db(const std::string& connection_str) {
    SQLHENV hEnv;
    SQLHDBC hDbc;
    SQLRETURN ret;

    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_NTS);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    auto jopa = 
    ret = SQLDriverConnectA(hDbc, NULL, (SQLCHAR*)(connection_str.c_str()), SQL_NTS,
        NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

    if (SQL_SUCCEEDED(ret)) {
        return hDbc;
    }
    else {
        std::cerr << "Failed to connect to database." << std::endl;
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        return SQL_NULL_HDBC;
    }
}

bool get_user_from_db(SQLHDBC hDbc, const std::string& fio, const std::string& specialty,
    const std::string& subject_scores, const studyType study_type, User& user) {

    SQLHSTMT hStmt;
    SQLRETURN ret;
    SQLLEN indicator;

    std::string query = "SELECT fio, specialty, subject_scores, study_type FROM users WHERE fio = '" +
        fio + "' AND specialty = '" +
        specialty + "' AND subject_scores = " +
        subject_scores + " AND study_type = '" +
        convertStudyTypeToStr(study_type) + "';";

    // Создаем указатель SQL-запроса
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    if (SQL_SUCCEEDED(ret)) {
        if (SQLFetch(hStmt) == SQL_SUCCESS) {
            char fioBuffer[256], specialtyBuffer[256], studyTypeBuffer[256];
            int subjectScoresValue;

            SQLGetData(hStmt, 1, SQL_C_CHAR, fioBuffer, sizeof(fioBuffer), &indicator);
            SQLGetData(hStmt, 2, SQL_C_CHAR, specialtyBuffer, sizeof(specialtyBuffer), &indicator);
            SQLGetData(hStmt, 3, SQL_C_SLONG, &subjectScoresValue, 0, &indicator);
            SQLGetData(hStmt, 4, SQL_C_CHAR, studyTypeBuffer, sizeof(studyTypeBuffer), &indicator);

            // Сохраняем данные пользователя
            user.fio = fioBuffer;
            user.specialty = specialtyBuffer;
            user.subjectScores = std::to_string(subjectScoresValue);
            user.study_type = studyTypeBuffer;

            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            return true;
        }
    }
    else {
        std::cerr << "Failed to execute query." << std::endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return false;
}

std::vector<User> get_all_users_from_db(SQLHDBC hDbc) {
    SQLHSTMT hStmt;
    SQLRETURN ret;
    std::vector<User> users;

    std::string query = "SELECT fio, specialty, subject_scores, study_type FROM users ORDER BY study_type, subject_scores desc";

    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    if (SQL_SUCCEEDED(ret)) {
        // Извлечение данных о пользователях
        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            User user;
            char fioBuffer[256], specialtyBuffer[256], studyTypeBuffer[256];
            int subjectScoresValue;

            SQLGetData(hStmt, 1, SQL_C_CHAR, fioBuffer, sizeof(fioBuffer), NULL);
            SQLGetData(hStmt, 2, SQL_C_CHAR, specialtyBuffer, sizeof(specialtyBuffer), NULL);
            SQLGetData(hStmt, 3, SQL_C_SLONG, &subjectScoresValue, 0, NULL);
            SQLGetData(hStmt, 4, SQL_C_CHAR, studyTypeBuffer, sizeof(studyTypeBuffer), NULL);

            user.fio = fioBuffer;
            user.specialty = specialtyBuffer;
            user.subjectScores = std::to_string(subjectScoresValue);
            user.study_type = studyTypeBuffer;

            users.push_back(user);
        }
    }
    else {
        std::cerr << "Failed to execute query." << std::endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return users;
}

bool delete_user_from_db() { return false; } // not implemented

bool add_user_to_db(SQLHDBC hDbc, const std::string& fio, const std::string& specialty,
    const std::string& subject_scores, const studyType study_type) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    User user;
    if (get_user_from_db(hDbc, fio, specialty, subject_scores, study_type, user)) {
        std::cout << "User already exist." << std::endl;
        return false;
    }
    std::string query = "INSERT INTO users (fio, specialty, subject_scores, study_type) VALUES ('" +
        fio + "', '" +
        specialty + "', " +
        subject_scores + ", '" +
        convertStudyTypeToStr(study_type)
        + "');";

    ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    if (SQL_SUCCEEDED(ret)) {
        std::cout << "User added successfully." << std::endl;
        return true;
    }
    else {
        std::cerr << "Failed to execute query." << std::endl;
        return false;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

std::string find_available_study_type(SQLHDBC hDbc, const std::string& fio, const studyType study_type) {

    auto it = std::find(priorityStudyTypes.begin(), priorityStudyTypes.end(), study_type);
    // Если элемент найден, начать обход с этого элемента до конца, пока не вернется true
    if (it != priorityStudyTypes.end()) {
        for (auto current = it; current != priorityStudyTypes.end(); ++current) {
            SQLHSTMT hStmt;
            SQLRETURN ret;
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

            std::string study_type_str = convertStudyTypeToStr(*current);
            std::string query =
                "WITH ranked_candidates AS( "
                "    select DISTINCT * from( "
                "        select * from users where study_type = '" + study_type_str + "' "
                "        union all "
                "        select * from users where fio = '" + fio + "' "
                "        order by subject_scores "
                "    ) "
                "    order by subject_scores desc "
                "    limit 5 "
                ") "
                "select EXISTS(select * from ranked_candidates where fio = '" + fio + "') ";

            ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);

            if (SQL_SUCCEEDED(ret)) {
                // Извлечение данных о пользователях
                if(SQLFetch(hStmt) == SQL_SUCCESS) {
                    bool isHaveVacation;
                    SQLCHAR booleanValue;

                    SQLRETURN retGetData = SQLGetData(hStmt, 1, SQL_C_BIT, &booleanValue, sizeof(booleanValue), NULL);

                    if (SQL_SUCCEEDED(retGetData)) {
                        isHaveVacation = booleanValue != 0;  // Преобразуем 0/1 в true/false
                        if (isHaveVacation)
                            return convertStudyTypeToStrRu(*current);
                    }
                    else {
                        std::cerr << "Failed to sql get data." << std::endl;
                    }
                }
            }
            else {
                std::cerr << "Failed to execute query." << std::endl;
            }

            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        }
    }
    else {
        // error std::err << "not find a priorityTypes"
    }  

    return "undefined";
}
