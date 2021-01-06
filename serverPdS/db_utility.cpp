//
// Created by simo on 06/12/20.
//

#include "db_utility.h"

bool db_utility::login_db(std::string username, std::string password) {
    sqlite3 *db;
    int rc;
    char *zErrMsg = 0;

    rc = sqlite3_open("db/server.db", &db);

    if (rc) {
        std::cout << "- Can't open database: " << sqlite3_errmsg(db) << std::endl;
    }

    std::string query_string =
            "select count(*) from user where username = '" + username + "' and password = '" + password + "'";
    const char *query = query_string.c_str();

    rc = sqlite3_exec(db, query, db_utility::check_login, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cout << "SQL error: " << sqlite3_errmsg(db) << "\n";
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}


int db_utility::check_login(void *NotUsed, int argc, char **argv, char **azColName) {
    if (std::string(argv[0]) == "1") {
        return 0;
    } else {
        std::cout << "Utente NON presente nel DB . . . ." << std::endl;
        return 1;
    }
}
