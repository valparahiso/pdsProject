//
// Created by simo on 06/10/20.
//

#include "Server.h"

Server::Server(boost::asio::io_service& io_service): acceptor_(io_service, tcp::endpoint(tcp::v4(), 8001))
{
    start_accept();
}

void Server::start_accept()
{
    // socket
    con_handler::pointer connection = con_handler::create(acceptor_.get_io_service());

    // asynchronous accept operation and wait for a new connection.
    acceptor_.async_accept(connection->socket(),
                           boost::bind(&Server::handle_accept, this, connection,
                                       boost::asio::placeholders::error));

}

void Server::handle_accept(con_handler::pointer connection, const boost::system::error_code& err)
{
    if (!err) {
        connection->start();
    }
    start_accept();
}