//
// Created by simo on 19/10/20.
//

#ifndef CLIENTPDS_USER_H
#define CLIENTPDS_USER_H

#include <iostream>
#include "parameters_exception.h"
#include "tcp_client.h"

class user {
public:
    user(std::string directory, std::string command) : directory(directory), command(command){
        if(!this->check_command()){
            std::cout<<"Comando errato."<<std::endl;
            throw parameters_exception();
        }
    }

    void authentication(){
        std::cout << "Please, enter your username: ";
        std::getline (std::cin,this->username);

        std::cout << "Please, enter your password: ";
        std::getline (std::cin,this->password);

        start_communication();
    }

    std::string get_username(){
        return this->username;
    }

    std::string get_password(){
        return this->password;
    }

    std::string get_directory(){
        return this->directory;
    }

    std::string get_command(){
        return this->command;
    }

private:
    std::string username;
    std::string password;
    std::string command;
    std::string directory;

    bool check_command(){
        if(this->command.compare("default")==0 || this->command.compare("check_validity")==0 || this->command.compare("restore")==0){
            return true;
        } return false;
    }

    void start_communication(){
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp_client client = tcp_client(io_context, username, password);

        client.start(resolver.resolve("127.0.0.1", "8001"));
        io_context.run();
    }

};


#endif //CLIENTPDS_USER_H
