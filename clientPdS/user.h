//
// Created by simo on 19/10/20.
//

#ifndef CLIENTPDS_USER_H
#define CLIENTPDS_USER_H

#include <iostream>
#include <boost/filesystem.hpp>
#include "parameters_exception.h"
#include "directory_exception.h"
#include "tcp_client.h"

class user {
public:
    user(std::string directory, std::string command) : directory(directory), command(command){
        check_command();
    }

    void connection(){
        authentication();
        start_communication();
    }

private:
    std::string username;
    std::string password;
    std::string command;
    std::string directory;

    void authentication(){
        std::cout << "Please, enter your username: ";
        std::getline (std::cin,this->username);

        std::cout << "Please, enter your password: ";
        std::getline (std::cin,this->password);
    }

    void check_command(){
        if(command=="restore"){
            return;
        } else if(command=="check_validity" || command == "default"){
            check_directory();
            return;
        }
        std::cout<<"Comando errato."<<std::endl;
        throw parameters_exception();
    }

    void check_directory(){
        boost::filesystem::path path(directory);
        if(exists(path)){
            if(is_directory(path)){
                directory = boost::filesystem::absolute(boost::filesystem::canonical(path)).string();
                return;
            }
        }
        throw directory_exception();
    }


    void start_communication(){
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp_client client = tcp_client(io_context, username, password, directory, command);

        client.start(resolver.resolve("127.0.0.1", "8001"));
        io_context.run();
    }

};


#endif //CLIENTPDS_USER_H
