#include <boost/asio/io_context.hpp>
#include "tcp_server.h"
#include "tcp_connection.h"
#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

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


