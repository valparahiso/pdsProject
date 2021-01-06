//
// Created by simo on 19/10/20.
//

#include "user.h"

user::user(std::string directory, std::string command) : directory(directory), command(command){
    check_command();
}

void user::connection(){
    authentication();
    start_communication();
}

void user::authentication(){
    std::cout << "Please, enter your username: ";
    std::getline (std::cin,this->username);

    std::cout << "Please, enter your password: ";
    std::getline (std::cin,this->password);
    std::cout<<std::endl;
}

void user::check_command(){
    if(command=="restore"){
        return;
    } else if(command=="check_validity" || command == "default"){
        check_directory();
        return;
    }
    std::cout<<"Comando errato."<<std::endl;
    throw parameters_exception();
}

void user::check_directory(){
    boost::filesystem::path path(directory);
    if(exists(path)){
        if(is_directory(path)){
            directory = boost::filesystem::absolute(boost::filesystem::canonical(path)).string();
            return;
        }
    }

    throw directory_exception();
}

void user::start_communication(){
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp_client client = tcp_client(io_context, username, password, directory, command);

    client.start(resolver.resolve("127.0.0.1", "8001"));
    io_context.run();
}


