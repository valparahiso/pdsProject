//
// Created by simo on 06/10/20.
//

#ifndef CLIENDPDS_CLIENT_H
#define CLIENDPDS_CLIENT_H


#include <string>

class Client {
private:
    std::string username;
    std::string password;
    std::string directory;

public:
    Client(std::string);
    std::string getDirectory();
    void setDirectory(std::string directory);

};


#endif //CLIENDPDS_CLIENT_H
