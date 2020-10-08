//
// Created by simo on 06/10/20.
//test

#include <iostream>
#include "Client.h"
Client::Client(std::string directory) : directory(directory){
    std::cout<<"Client created with directory "<<this->directory<<std::endl;
};

std::string Client::getDirectory(){
    return directory;
}

void Client::setDirectory(std::string directory){
    this->directory = directory;
}
