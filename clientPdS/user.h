

#ifndef CLIENTPDS_USER_H
#define CLIENTPDS_USER_H

#include <iostream>
#include <boost/filesystem.hpp>
#include "parameters_exception.h"
#include "directory_exception.h"
#include "tcp_client.h"

class user {
public:
    user(std::string directory, std::string command);

    void connection();

private:
    std::string username;
    std::string password;
    std::string command;
    std::string directory;

    bool authentication();

    void check_command();

    void check_directory();


    void start_communication();

};
#endif //CLIENTPDS_USER_H
