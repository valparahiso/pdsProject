//
// Created by simo on 19/10/20.
//

#include "user.h"

user::user(std::string directory, std::string command) : directory(directory), command(command){
    //Controllare se il comando inserito è corretto
    check_command();
}

void user::connection(){
    //Authentication: richiedo i parametri di login all'utente

    //Start_communication:  inizio la connessione con il server
    if(authentication())
        start_communication();
}

bool user::authentication(){
    std::cout << "Please, enter your username: ";
    std::getline (std::cin,this->username);
    if(this->username.find('/') != std::string::npos){
        std::cout<<"Errore username: carattere non valido"<<std::endl;
        return false;
    }

    std::cout << "Please, enter your password: ";
    std::getline (std::cin,this->password);
    std::cout<<std::endl;
    if(this->username.find('/') != std::string::npos){
        std::cout<<"Errore username: carattere non valido"<<std::endl;
        return false;
    }
    return true;
}

void user::check_command(){
    if(command=="restore"){
        return;
    } else if(command=="check_validity" || command == "default"){
        /*Check_directory: se il comando inserito è uno dei due controllo che la cartella
        *                  esita e sia valida (directory e non file)
        */
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

    //Tcp_client :
    tcp_client client = tcp_client(io_context, username, password, directory, command);

    client.start(resolver.resolve("127.0.0.1", "8001"));
    io_context.run();
}


