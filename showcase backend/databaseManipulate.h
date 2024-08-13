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

