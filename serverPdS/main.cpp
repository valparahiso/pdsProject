#include <iostream>
#include <boost/asio.hpp>
#include "Server.h"

int main(int argc, char *argv[])
{
    try
    {
        boost::asio::io_service io_service;
        Server server(io_service);
        io_service.run();
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << endl;
    }
    return 0;
}