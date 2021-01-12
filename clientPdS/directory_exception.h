
#ifndef CLIENTPDS_DIRECTORY_EXCEPTION_H
#define CLIENTPDS_DIRECTORY_EXCEPTION_H

#include <iostream>
#include <exception>

class directory_exception: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Directory non esistente!";
    }
};



#endif //CLIENTPDS_DIRECTORY_EXCEPTION_H
