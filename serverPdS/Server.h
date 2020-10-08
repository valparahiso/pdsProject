//
// Created by simo on 06/10/20.
//

#ifndef SERVERPDS_SERVER_H
#define SERVERPDS_SERVER_H
//importing libraries
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "con_handler.h"

class Server
{
    private:
        tcp::acceptor acceptor_;
        void start_accept();
    public:
    //constructor for accepting connection from client
        Server(boost::asio::io_service& io_service);
        void handle_accept(con_handler::pointer connection, const boost::system::error_code& err);
};


#endif //SERVERPDS_SERVER_H
