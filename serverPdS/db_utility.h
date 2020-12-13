//
// Created by simo on 06/12/20.
//

#ifndef SERVERPDS_DB_UTILITY_H
#define SERVERPDS_DB_UTILITY_H

#include <iostream>
#include <string>
#include <sqlite3.h>


namespace db_utility {


    bool login_db(std::string username, std::string password);

    int check_login(void *NotUsed, int argc, char **argv, char **azColName);


};


#endif //SERVERPDS_DB_UTILITY_H
