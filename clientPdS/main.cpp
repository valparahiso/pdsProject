#include <iostream>
#include "user.h"
#include "parameters_exception.h"

int main(int argc, char* argv[])
{
    try
    {
        if(argc < 2 || argc > 3) { // user.exe "nome directory" "comando"

            throw parameters_exception();
        }
        user u = (argc == 2) ? user(std::string(argv[1]), "default") : user(std::string(argv[1]), std::string(argv[2]));

        u.connection();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

