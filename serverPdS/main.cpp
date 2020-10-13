#include <boost/asio/io_context.hpp>
#include "tcp_server.h"
#include <iostream>
#include <string>


using boost::asio::ip::tcp; 



int main(){
    try{
        boost::asio::io_context io_context;
        tcp_server server(io_context);
        io_context.run();
    }
    catch (std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    return 0;
}


