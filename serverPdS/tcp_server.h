

#ifndef PDSPROJECT_TCP_SERVER_H
#define PDSPROJECT_TCP_SERVER_H
#include "tcp_connection.h"
#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;


class tcp_server {
public:
    tcp_server(boost::asio::io_context &io_context);

private:
    void start_accept();

    void handle_accept(tcp_connection::pointer new_connection,
                       const boost::system::error_code &error);

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;


};
#endif //PDSPROJECT_TCP_SERVER_H
